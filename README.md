# Medidor de pressão utilizando uma Lilygo TTGO ESP32 com display OLED integrado

Medidor de pressão utilizando uma Lilygo TTGO ESP32 com display OLED integrado e sensores BMP280, de temperatura e pressão barométrica, e SMP3011, de pressão.

# Antes de começar

- O código foi projetado para a IDE ESP-IDF - que depende do Python - no Visual Studio Code, e utilizando-se do CMake junto às C++ Build Tools do Visual Studio 2019, portanto, todos estes serão necessários para a compilação;
- É necessário que o sistema possua as variáveis de ambiente `PATH`, contendo o diretório `/.espressif/` e `%IDF_PATH/tools`, e `IDF_PATH`, contendo o diretório onde está instalado o ESP-IDF, para que o código do projeto carregue todas as dependências;

Para funcionamento dos sensores e display:
- Abra o temrminal do Visual Studio Code e execute `git clone --recursive https://github.com/eng-software/ComponentesESP32.git components\tex` para obter os componentes herdados para o projeto.
