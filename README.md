# Лабораторная работа №3  
## «Обфускация строк в программном обеспечении с использованием ADVobfuscator»

Дисциплина: «Защита программного обеспечения защищенных автоматизированных систем»  

Выполнил: Погосян Николай

---

## Цель работы

1. Освоить базовые приёмы обфускации строк в исполняемом файле.
2. Освоить работу с библиотекой ADVobfuscator (legacy-версия).
3. Сравнить бинарный код программы до и после обфускации с помощью дизассемблера IDA Pro.
4. Сделать вывод о влиянии обфускации на сложность анализа программы.

---

## Используемое программное обеспечение

- Windows 10 Pro (виртуальная машина VirtualBox).
- Microsoft Visual Studio 2022 Community, workload  
  «Разработка классических приложений на C++».
- ADVobfuscator (legacy-версия, каталог `Lib`).
- Дизассемблер IDA Pro 9.2 (x64).
- Командная строка Windows (cmd.exe).

---

## Структура репозитория

```
ZPOZAS_LAB3/
│
├── src/
│   └── LR3_Obfuscation.cpp   # исходный код программы с обфускацией строк
│
├── ADVobfuscator/
│   └── Lib/                  # заголовочные файлы ADVobfuscator-legacy (MetaString.h и др.)
│
└── README.md                 # отчёт по лабораторной работе
```
---

## Ход работы
### 1. Создание и отладка базовой программы

В Visual Studio создан проект типа
«Консольное приложение (C++)» с именем LR3_Obfuscation.

В качестве рабочего каталога проекта использована папка:
```
C:\LR3\LR3_Obfuscation\
```
---

В файл LR3_Obfuscation.cpp был добавлен следующий код:
```cpp
#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h>

int main()
{
    // Переключаем кодировку консоли на UTF-8
    system("chcp 65001 > nul");

    // 1. Системный вызов: вывод сообщения через cmd
    system("cmd /c echo --- CMD launch completed ---");

    // 2. Вывод информации об авторе
    std::cout << "Author: Pogosyan Nikolai" << std::endl;

    // 3. Формирование пути к каталогу Documents текущего пользователя
    std::string path = std::string("C:\\Users\\") +
                       getenv("USERNAME") +
                       "\\Documents";

    std::cout << "\nFolder content: " << path << std::endl;

    // 4. Перебор файлов и папок в каталоге Documents
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            std::cout << " - "
                      << entry.path().filename().string()
                      << std::endl;
        }
    }
    catch (...)
    {
        std::cerr << "Error: cannot read directory." << std::endl;
    }

    std::cout << "\nDone." << std::endl;

    // 5. Задержка окна консоли
    system("pause");
    return 0;
}
```

---

Программа была собрана и запущена (Debug / x64, запуск без отладки).
На экран выводится:

```cmd
сообщение о запуске CMD;

строка Author: Pogosyan Nikolai;

путь к каталогу C:\Users\USER\Documents;

перечисление файлов и папок в каталоге Documents;

Done.
Press any key to continue....

```

Полученная программа корректно выполняет требования методических указаний:
выполняет системный вызов, выводит фамилию/имя автора и список файлов в каталоге
Documents.

---

### 2. Подключение библиотеки ADVobfuscator (legacy)

Был скачан архив ADVobfuscator-legacy.zip и распакован.

Внутри архива используется следующая структура:
```
ADVobfuscator-legacy/
    DocCode/
    Docs/
    Examples/
    Lib/          # папка с заголовочными файлами
    ADVobfuscator.sln
    README.md
```
В каталог проекта C:\LR3\LR3_Obfuscation\ была скопирована папка:

```
ADVobfuscator\Lib\ 
```

Внутри неё находятся файлы:
```
⦁	Indexes.h
⦁	Inline.h
⦁	Log.h
⦁	MetaFSM.h
⦁	MetaRandom.h
⦁	MetaString.h
⦁	ObfuscatedCall.h
⦁	ObfuscatedCallWithPredicate.h
⦁	Unroller.h
```

В свойствах проекта Visual Studio добавлен путь к заголовочным файлам библиотеки:

Свойства проекта → C/C++ → Общие → «Дополнительные каталоги включаемых файлов»

Добавлено значение:
```
$(ProjectDir)ADVobfuscator\Lib
```

> Для отключения предупреждений о «небезопасных» CRT-функциях (getenv) в разделе
C/C++ → Препроцессор в список определений препроцессора добавлен символ:
```
_CRT_SECURE_NO_WARNINGS
```

---

### 3. Обфускация строк в программе

После подключения библиотеки код был модифицирован с использованием макроса
OBFUSCATED, предоставляемого ADVobfuscator-legacy.

Финальный вариант файла LR3_Obfuscation.cpp:
```cpp
#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h>

// ADVobfuscator (legacy)
#include "MetaString.h"
#include "Log.h"

using namespace andrivet::ADVobfuscator;

int main()
{
    // 1. System call: change console code page (UTF-8)
    system(OBFUSCATED("chcp 65001 > nul"));

    // 2. System call: CMD launch message
    system(OBFUSCATED("cmd /c echo --- CMD launch completed ---"));

    // 3. Output author name (obfuscated string)
    std::cout << OBFUSCATED("Author: Pogosyan Nikolai") << std::endl;

    // 4. Path to Documents (parts are obfuscated)
    std::string path =
        std::string(OBFUSCATED("C:\\Users\\")) +
        getenv("USERNAME") +
        OBFUSCATED("\\Documents");

    std::cout << std::endl
              << OBFUSCATED("Folder content: ") << path << std::endl;

    // 5. List directory content
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            std::cout << " - "
                      << entry.path().filename().string()
                      << std::endl;
        }
    }
    catch (...)
    {
        std::cerr << OBFUSCATED("Error: cannot read directory.")
                  << std::endl;
    }

    std::cout << std::endl << OBFUSCATED("Done.") << std::endl;

    // 6. Pause (system call)
    system(OBFUSCATED("pause"));

    return 0;
}
```

Все пользовательские строки (сообщения, путь, текст ошибки) передаются в макрос OBFUSCATED. На этапе компиляции строки шифруются и в явном виде в бинарном файле не хранятся. Расшифровка выполняется только в момент использования строки.


---

### 4. Сборка обфусцированной версии (Release)

В Visual Studio выбрана конфигурация:
```
Конфигурация: Release

Платформа: x64

Для конфигурации Release в определениях препроцессора удалён символ _DEBUG, чтобы избежать линковки с отладочными версиями CRT-функций.
```
Проект успешно собран. Полученный исполняемый файл:
```
C:\LR3\LR3_Obfuscation\x64\Release\LR3_Obfuscation.exe
```


При запуске из консоли программа выводит:
```
--- CMD launch completed ---
Author: Pogosyan Nikolai

Folder content: C:\Users\USER\Documents
 - desktop.ini
 - Visual Studio 18
 - ...

Done.
Press any key to continue . . .
```


<img width="391" height="245" alt="image" src="https://github.com/user-attachments/assets/407532b7-8c29-4019-9d19-bf90d4f5d06e" />

Функциональность полностью совпадает с первоначальной версией без обфускации, что подтверждает корректность использования библиотеки ADVobfuscator.

---

### 5. Анализ обфусцированного файла в IDA Pro

Обфусцированный файл LR3_Obfuscation.exe был загружен в IDA Pro 9.2 (x64).

#### 5.1. Анализ функции main

IDA корректно определила функцию main.

<img width="1070" height="892" alt="image" src="https://github.com/user-attachments/assets/b827e6c0-d304-4751-8dcf-445cc3caba6d" />

ДО ОБФУСКАЦИИ было:
<img width="1067" height="884" alt="image" src="https://github.com/user-attachments/assets/b3242c93-f02d-40cb-8564-1949d50d5d16" />



Граф функции стал более громоздким по сравнению с простой версией без обфускации:
появляются дополнительные вспомогательные конструкции, связанные с работой std::filesystem и макросом OBFUSCATED.

Тем не менее логика программы всё ещё читаема: видны вызовы
std::filesystem::directory_iterator, обращения к std::cout, system и т.д.

#### 5.2. Анализ строк (Strings)

ДО ОБФУСКАЦИИ было:
<img width="1069" height="885" alt="image" src="https://github.com/user-attachments/assets/bfd3a646-df34-441e-b433-0c0e1481573e" />


ПОСЛЕ:
<img width="1072" height="890" alt="image" src="https://github.com/user-attachments/assets/3a5775fd-a44a-49a5-9b5a-9c0c4be4b2a4" />

В окне Strings отсутствуют строки:
```
Author: Pogosyan Nikolai

Folder content:

Error: cannot read directory.

Done.
```

и другие текстовые сообщения программы.

В списке строк в основном присутствуют:

* служебные строки стандартной библиотеки (std::out_of_range, MSVCP140.dll и т.п.);
* строки, относящиеся к инфраструктуре исключений и рантайма.

Это означает, что пользовательские строки успешно зашифрованы и не хранятся в открытом виде в секции .rdata.

#### 5.3. Hex-анализ

ДО ОБФУСКАЦИИ было:
<img width="1071" height="890" alt="image" src="https://github.com/user-attachments/assets/f172c871-7908-4c5a-9078-9ee5c23252ff" />

ПОСЛЕ:
<img width="1073" height="885" alt="image" src="https://github.com/user-attachments/assets/4daa4beb-ee00-4934-8a0d-ddcb3ed9e48f" />


В окне Hex View по адресам, соответствующим данным, вместо читаемых ASCII-строк наблюдаются «мусорные» последовательности байт, не интерпретируемые IDA как строки.

Это соответствует ожиданиям: макрос OBFUSCATED генерирует на этапе компиляции зашифрованные константы, которые потом расшифровываются во время выполнения.

---

### Выводы

Была разработана и отлажена простая консольная программа на C++, которая:

* выполняет системный вызов через system();
* выводит информацию об авторе;
* формирует путь к каталогу Documents текущего пользователя;
* перечисляет все файлы и папки в данном каталоге.

В проект была интегрирована библиотека ADVobfuscator (legacy), в том числе:

* добавлены заголовочные файлы из каталога Lib;
* настроены пути к заголовочным файлам в Visual Studio;
* отключены лишние предупреждения компилятора.

Все пользовательские строковые литералы были обфусцированы с использованием макроса OBFUSCATED. При этом:

* поведение программы для пользователя не изменилось;
* строки перестали присутствовать в явном виде в бинарном файле.

Анализ обфусцированного исполняемого файла в IDA Pro показал, что:

* в окне Strings отсутствуют читаемые пользовательские строки;
* вместо этого в данных находятся зашифрованные байтовые массивы;
* структура функции main усложнилась, что затрудняет статический анализ.

Обфускация строк с помощью ADVobfuscator существенно усложняет поиск текстовых артефактов в исполняемом файле и повышает трудоёмкость обратного анализа программы. При этом накладные расходы на реализацию обфускации в коде минимальны.

Работа по ЛР №3 выполнена, цель лабораторной работы достигнута.
