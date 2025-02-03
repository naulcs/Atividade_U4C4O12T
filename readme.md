# Projeto: Display Numérico com WS2812 e Raspberry Pi Pico

## Descrição
Este projeto implementa um display numérico utilizando LEDs WS2812 controlados pelo Raspberry Pi Pico. Os números de 0 a 9 são representados em uma matriz 5x5 de LEDs RGB. O usuário pode alternar entre os números pressionando botões físicos conectados ao microcontrolador.

## Componentes Utilizados
- Raspberry Pi Pico
- 25 LEDs WS2812 (endereçáveis individualmente)
- 2 botões push-button
- 1 LED RGB com pinos de controle individuais (Vermelho, Verde e Azul)
- Resistores Pull-up (se necessário)

## Pinos Utilizados
| Função         | Pino do Pico |
|----------------|--------------|
| WS2812 Data   | GP7          |
| Botão A       | GP5          |
| Botão B       | GP6          |
| LED RGB Vermelho | GP13         |
| LED RGB Verde    | GP11         |
| LED RGB Azul     | GP12         |

## Funcionamento
1. Os LEDs WS2812 formam uma matriz 5x5 para exibir números.
2. O botão A incrementa o número exibido.
3. O botão B decrementa o número exibido.
4. Um LED RGB pisca periodicamente para indicar funcionamento.

## Bibliotecas Necessárias
O projeto depende das bibliotecas da Raspberry Pi Pico SDK:
- `pico/stdlib.h`
- `hardware/pio.h`
- `hardware/clocks.h`
- `ws2812.pio.h`

## Como Compilar e Executar
1. Instale o Raspberry Pi Pico SDK.
2. Clone este repositório e navegue até o diretório do projeto.
3. Compile o código usando CMake:
   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```
4. Envie o arquivo `.uf2` gerado para o Raspberry Pi Pico.
5. O display iniciará com o número `0` e pode ser alterado pelos botões.

## Possíveis Melhorias
- Adicionar mais efeitos de transição entre os números.
- Implementar um sensor para mudar os números automaticamente.
- Criar padrões personalizáveis para exibição de caracteres adicionais.

## Autor
Desenvolvido por [Luan Silva}

## Demonstração do Projeto 🎥

[🎬 Assista ao vídeo](Atividade_U4C4O12T/VideoDeDemo.mp4)
