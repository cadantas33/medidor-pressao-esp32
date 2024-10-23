# Medidor de pressão utilizando uma Lilygo TTGO ESP32 com display OLED integrado

Medidor de pressão utilizando uma Lilygo TTGO ESP32 com display OLED integrado e sensores BMP280, de temperatura e pressão barométrica, e SMP3011, de pressão.

# Antes de começar

- O código foi projetado para a IDE ESP-IDF - que depende do Python - no Visual Studio Code, e utilizando-se do CMake junto às C++ Build Tools do Visual Studio 2019, portanto, todos estes devem estar instalados antes de clonar o repositório;
- É necessário que o sistema possua as variáveis de ambiente `PATH`, contendo o diretório `/.espressif/` e `%IDF_PATH/tools`, e `IDF_PATH`, contendo o diretório onde está instalado o ESP-IDF, para que o código do projeto carregue todas as dependências;

Para funcionamento dos sensores e display:
- Abra o temrminal do Visual Studio Code e execute `git clone --recursive https://github.com/eng-software/ComponentesESP32.git components\tex` para obter os componentes herdados para o projeto;

Para carregar o software para a placa ESP32:

- Na aba da extensão do IDF, selectione `Set Espressif Target`;
- Selecione a primeira opção, `esp32`;
- Selecione a última opção, `Custom board`.

# Detalhe adicional

  Verifique se os drivers de comunicação serial se encontram instalados em sua máquina.
  Caso não estejam, instale-os [aqui](https://www.silabs.com/documents/public/software/CP210x_Universal_Windows_Driver.zip).
