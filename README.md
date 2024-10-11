# Medidor de pressão utilizando uma Lilygo TTGO ESP32 com display OLED integrado

Medidor de pressão utilizando uma Lilygo TTGO ESP32 com display OLED integrado e sensores BMP280, de temperatura e pressão barométrica, e SMP3011, de presssão.

Caso ocorram erros quanto às bibliotecas e headers incluídas logo no início do código, fazer o seguinte:

- Passe o mouse por cima da linha sublinhada
- Clique no pequeno ícone de lâmpada e depois em "Edit include path"
- Nas configurações da extensão "C/C++", rolar até "Include path" e incluir os diretórios das header files dos sensores;
- Por ex: se a pasta "BMP280", contendo os arquivos para o funcionamento do sensor, estiver localizada em 'C:\[PastaOndeEstáOProjeto]\medidor-pressao-esp32\BMP280', este será o diretório a ser incluído no espaço "Include path"

![image](https://github.com/user-attachments/assets/e170e795-0b20-40e8-a9d3-2bce0ae1d71b)

