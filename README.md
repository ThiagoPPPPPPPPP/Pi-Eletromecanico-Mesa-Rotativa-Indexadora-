# Mesa Giratória Indexadora - Módulo de Ensino de Mecatrônica

Este repositório contém a documentação e os arquivos de projeto da **Mesa Giratória Indexadora**, um módulo de ensino desenvolvido para auxiliar na formação prática de alunos sobre integração de sistemas automatizados.

O projeto integra mecânica, eletrônica e programação para simular um processo industrial de indexação, detecção e separação de peças.

---

## 📖 Sobre o Projeto

A **Mesa Giratória Indexadora** foi projetada como um recurso pedagógico para demonstrar princípios de automação industrial, incluindo:
* Controle de posicionamento preciso.
* Sensoriamento e identificação de materiais.
* Atuação eletromecânica.
* Lógica de comando sequencial.

O sistema opera com uma tensão de segurança (5V a 24V) e possui uma estrutura transparente (acrílico) para permitir a visualização clara dos componentes e do funcionamento interno durante as aulas.

### ⚙️ Funcionalidades
* **Indexação Automática:** Movimentação da mesa em 6 posições fixas com parada precisa.
* **Detecção de Material:** Identificação de peças metálicas e não-metálicas (plástico) utilizando sensores indutivos e capacitivos.
* **Sistema de Expulsão:** Atuador solenoide customizado para remover peças da linha de produção após o processamento.
* **Interface Visual:** Feedback de status dos sensores e etapas do ciclo através de LEDs.
* **Segurança:** Operação em baixa tensão e botão de emergência.

---

## 🛠️ Hardware e Arquitetura

O projeto utiliza uma arquitetura dividida em **Nível Inferior** (Controle e Potência) e **Nível Superior** (Atuação e Processamento).

### Componentes Eletrônicos Principais
* **Controlador:** Arduino Mega 2560 R3.
* **Shield PLC:** Placa de adaptação DNMEB50 da eletechsup.
* **Atuadores:**
    * Motor de Passo NEMA 17.
    * Driver de Motor TB6600.
    * Solenoide Customizada (Ejeção).
    * Cooler 12V para refrigeração da eletrônica.
* **Sensores:**
    * Sensores Indutivos (Detecção de metal).
    * Sensores Capacitivos (Detecção de presença/não-metal).
* **Alimentação:** Fonte Chaveada 24V/15A.

### Estrutura Mecânica
* **Base e Caixa:** Acrílico 6mm (Corte a Laser).
* **Mesa Giratória:** Acrílico 4mm.
* **Peças de Suporte:** Impressão 3D (ABS) para suportes de sensores, solenoide e engrenagens.
* **Transmissão:** Sistema de Pinhão e Engrenagem Interna com redução 1:5.

---

## 💻 Software e Lógica

O firmware foi desenvolvido em **C++ (Arduino)** e implementa uma máquina de estados para controlar o ciclo da máquina:

1.  **Detecção:** Aguarda peça na estação de entrada.
2.  **Identificação:** Lê sensores para classificar o material (Metal/Não Metal).
3.  **Movimentação:** Aciona o motor de passo para avançar a mesa para a próxima estação.
4.  **Expulsão:** Ao chegar na estação de saída, ativa a solenoide se a peça for confirmada.
5.  **Feedback:** Atualiza os LEDs indicadores de status em tempo real.

---

## 📦 Como Reproduzir

### Pré-requisitos
* Arduino IDE instalado.
* Bibliotecas padrão do Arduino.

### Instalação
1.  Clone este repositório:
    ```bash
    git clone [https://github.com/seu-usuario/mesa-giratoria-indexadora.git](https://github.com/ThiagoPPPPPPPPP/mesa-giratoria-indexadora.git)
    ```
2.  Abra o arquivo principal do código (ex: `Mesa_Indexadora.ino`) na Arduino IDE.
3.  Conecte o Arduino Mega via USB.
4.  Selecione a placa **Arduino Mega or Mega 2560** no menu de ferramentas.
5.  Faça o upload do código.

> **Nota:** Consulte o arquivo `Manual de Montagem.pdf` para o passo a passo da montagem mecânica e o esquema elétrico detalhado no arquivo do relatório.

---

## 👥 Autores

Projeto desenvolvido por alunos do curso de **Engenharia Mecatrônica** do **IFSC - Câmpus Florianópolis**:

* **Gabriel Vitor Salasario Boing**
* **Jorge Henrique Martins de Oliveira Chaves**
* **Sarah Even Oliveira Barbosa Fernandes**
* **Thiago Paulo Pohlenz Piccoli de Oliveira**

---

## 🏫 Instituição

**Instituto Federal de Santa Catarina (IFSC)**
Departamento Acadêmico de Metal Mecânica
Florianópolis - 2025

---

*Este projeto é de cunho educacional e open-source.*
