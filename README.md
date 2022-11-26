# cpp-transport-catalogue


## Проект курса Яндекс Практикум **«Разработчик С++»**
Финальный проект: **Транспортный справочник**

***`TransportCatalogue`*** - транспортный справочник. Работает с **JSON**-запросми. Выдаёт ответ на запрос отрисовки маршрутов строкой **SVG** формата. Реализован конструктор **JSON** с использованием вызова цепочки методов. Явные ошибки выявляются на этапе компиляции. Для сериализации и десериализации ***транспортного справочника*** применяется **Google Protocol Buffers(*Protobuf*)**.
___

### Системные требования:
+ [x] **C++17 и выше**
+ [x] **Protobuf 3.21 и выше** 
___

### Установка **Protobuf**
Для сборки проекта необходимо установить `Google Protocol Buffers`. Для этого:
1.  Скачать `Protobuf` можно с репозитория на GitHub. Выберите архив [protobuf-cpp](https://github.com/protocolbuffers/protobuf/releases) с исходным кодом последней версии и распакуйте его. Исходный код содержит  CMake-проект.
2.  Создать папки ***build-debug*** и ***build-release*** для сборки двух конфигурций Protobuf. При использовании  **Visual Studio**, достаточно создать папку ***build***. При использовании **CLion** или **QT Creator**, **IDE** автоматически создаст папки при открытии файла ***CMakeLists.txt***.
3.  Создать папку, в которой будет размещен пакет **Protobuf**. Для примера будем называть её ***/path/to/protobuf/package*** (далее следует указывать **абсолютный путь**).
Если сборка будет производиться не через IDE, в папке ***Debug*** необходимо выполнить следующие команды:
```
cmake <путь к protobuf>/cmake -DCMAKE_BUILD_TYPE=Debug \
      -Dprotobuf_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=/path/to/protobuf/package
cmake --build . 
```
где:
+ `Dprotobuf_BUILD_TESTS=OFF` - команда, которая выключает сборку тестов(экономим время);
+ `DCMAKE_INSTALL_PREFIX=/path/to/protobuf/package` - адрес папки для установки пакета **Protobuf**. 
Для **Visual Studio** конфигурация куазывается не при генерации, а при сборке:
```
cmake <путь к protobuf>/cmake ^
      -Dprotobuf_BUILD_TESTS=OFF ^
      -DCMAKE_INSTALL_PREFIX=/path/to/protobuf/package ^
      -Dprotobuf_MSVC_STATIC_RUNTIME=OFF
cmake --build . --config Debug 
```
После сборки библиотеки запустить команду: `cmake --install .` Под **Visual Studio** необходимо указать конфигурацию, т.к. она не задавалась во время генерации: `cmake --install . --config Debug`

Далее повторить теже шаги с конфигурацией **Release**.

**Важно!!!** При сборке проекта, необходимо указать путь к ***protobuf***: `DCMAKE_PREFIX_PATH=/path/to/protobuf/package`
___
### Сборка
С помощью **CMake** собрать файл ***CMakeLists.txt***.
___

### Аргументы командной строки (**cmd**) для запуска программы

`make_base` - создание бфзы транспортного справочника по запросам и её сериализация в файл с помощью ***Protobuf***.

`process_requests` - десериализация базы из файла и использование её для ответов на запросы `stat_requests`.
