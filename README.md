# Medidor de pressão utilizando uma Lilygo TTGO ESP32 com display OLED integrado

Medidor de pressão utilizando uma Lilygo TTGO ESP32 com display OLED integrado e sensores BMP280, de temperatura e pressão barométrica, e SMP3011, de pressão.

# Antes de começar

É necessário que o sistema possua as variáveis de ambiente `PATH` e `IDF_PATH`, para que o código do projeto carregue todas as dependências.

Para funcionamento dos sensores e display:
- Abra o temrminal do Visual Studio Code e execute `git clone --recursive https://github.com/eng-software/ComponentesESP32.git components\tex` para obter os componentes herdados para o projeto.
