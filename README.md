# SecureLens

Plataforma em C/C++ que combina análise de vulnerabilidades em dependências com classificação de sentimentos utilizando Machine Learning.

## Objetivo

O SecureLens é um projeto de portfólio que demonstra competências em:

- **Cybersecurity**: Scanner de vulnerabilidades em dependências de projetos, utilizando a API OSV (Open Source Vulnerabilities).
- **Machine Learning**: Classificador de sentimentos implementado do zero em C++, utilizando TF-IDF + Logistic Regression.

## Arquitetura

```
                     SecureLens
                         |
           +-------------+-------------+
           |                           |
    Security Module                ML Module
           |                           |
    Dependency Parser             Tokenizer
           |                           |
      OSV Client                     TF-IDF
           |                           |
   Vulnerability Data          Logistic Regression
           |                           |
           +-------------+-------------+
                         |
                     REST API
                         |
                       Client
```

## Tecnologias

- **C++17** - Linguagem principal
- **C11** - Componentes de baixo nível
- **CMake** - Sistema de build
- **libcurl** - Cliente HTTP para API OSV
- **nlohmann/json** - Parsing JSON
- **tinyxml2** - Parsing XML (pom.xml)
- **Docker** - Containerização
- **GitHub Actions** - CI/CD

## Estrutura do projeto

```
securelens/
├── CMakeLists.txt
├── Dockerfile
├── docker-compose.yml
├── include/
│   ├── api/
│   │   ├── security_routes.hpp
│   │   └── sentiment_routes.hpp
│   ├── security/
│   │   ├── scanner.hpp
│   │   ├── osv_client.hpp
│   │   ├── dependency.hpp
│   │   └── parsers/
│   │       ├── requirements_parser.hpp
│   │       ├── package_json_parser.hpp
│   │       └── pom_parser.hpp
│   └── ml/
│       ├── tokenizer.hpp
│       ├── tfidf.hpp
│       ├── logistic_regression.hpp
│       ├── classifier.hpp
│       └── model.hpp
├── src/
│   ├── main.cpp
│   ├── api/
│   │   ├── security_routes.cpp
│   │   └── sentiment_routes.cpp
│   ├── security/
│   │   ├── scanner.cpp
│   │   ├── osv_client.cpp
│   │   └── parsers/
│   │       ├── requirements_parser.cpp
│   │       ├── package_json_parser.cpp
│   │       └── pom_parser.cpp
│   └── ml/
│       ├── tokenizer.cpp
│       ├── tfidf.cpp
│       ├── logistic_regression.cpp
│       ├── model.cpp
│       └── classifier.cpp
├── cli/
│   └── main.cpp
├── training/
│   ├── dataset/
│   │   └── sentiment.csv
│   ├── train.cpp
│   └── evaluate.cpp
├── tests/
│   ├── test_main.cpp
│   ├── security_tests.cpp
│   ├── parser_tests.cpp
│   ├── ml_tests.cpp
│   └── api_tests.cpp
├── models/
│   └── .gitkeep
└── .github/
    └── workflows/
        └── ci.yml
```

## Requisitos

- CMake 3.16+
- GCC 9+ ou Clang 10+
- libcurl
- nlohmann/json
- tinyxml2

### Instalação das dependências (Debian/Ubuntu)

```bash
sudo apt-get install -y build-essential cmake libcurl4-openssl-dev nlohmann-json3-dev libtinyxml2-dev
```

## Compilação

```bash
cmake -S . -B build
cmake --build build
```

## Treinamento do modelo

```bash
./build/securelens-train
```

O treinamento:
1. Carrega o dataset de `training/dataset/sentiment.csv`
2. Divide em treino/teste (80/20)
3. Constrói vocabulário e calcula TF-IDF
4. Treina Logistic Regression
5. Avalia o modelo (accuracy, precision, recall, F1)
6. Salva o modelo em `models/sentiment_model.txt`

## Execução da API

```bash
./build/securelens
```

A API estará disponível em `http://localhost:8080`.

## Execução da CLI

```bash
# Scan de vulnerabilidades
./build/securelens-cli scan ./meu-projeto

# Análise de sentimento
./build/securelens-cli sentiment "This product is excellent."

# Análise combinada
./build/securelens-cli analyze ./meu-projeto "The application crashes frequently."
```

## Docker

```bash
docker compose up --build
```

## Testes

```bash
ctest --test-dir build
```

## Endpoints

### GET /health

Verifica se a API está funcionando.

**Resposta:**
```json
{"status":"healthy"}
```

### POST /api/v1/security/scan

Escaneia um repositório ou diretório em busca de vulnerabilidades.

**Request:**
```json
{
  "repository": "https://github.com/exemplo/projeto"
}
```

ou

```json
{
  "path": "/workspace/project"
}
```

**Resposta:**
```json
{
  "repository": "exemplo",
  "dependencies": 10,
  "vulnerabilities": 2,
  "findings": [
    {
      "id": "CVE-XXXX-XXXX",
      "cve": "CVE-XXXX-XXXX",
      "severity": "HIGH",
      "summary": "Example vulnerability",
      "fixed_version": "2.13.5",
      "package": "jackson-databind",
      "installed_version": "2.13.0"
    }
  ]
}
```

### POST /api/v1/sentiment

Classifica o sentimento de um texto.

**Request:**
```json
{
  "text": "The application is excellent and easy to use."
}
```

**Resposta:**
```json
{
  "sentiment": "positive",
  "confidence": 0.94
}
```

### POST /api/v1/analyze

Executa análise de segurança e sentimento simultaneamente.

**Request:**
```json
{
  "repository": "https://github.com/exemplo/projeto",
  "text": "The application is slow and crashes frequently."
}
```

**Resposta:**
```json
{
  "security": {
    "dependencies": 10,
    "vulnerabilities": 2,
    "findings": []
  },
  "sentiment": {
    "sentiment": "negative",
    "confidence": 0.91
  }
}
```

## Exemplos

### Scan de um projeto local

```bash
curl -X POST http://localhost:8080/api/v1/security/scan \
  -H "Content-Type: application/json" \
  -d '{"path": "./meu-projeto"}'
```

### Análise de sentimento

```bash
curl -X POST http://localhost:8080/api/v1/sentiment \
  -H "Content-Type: application/json" \
  -d '{"text": "I love this product!"}'
```

## Segurança

- O scanner **NÃO executa** código do repositório analisado
- O scanner **NÃO executa** scripts, binários ou package managers
- O scanner **NÃO compila** o código do projeto analisado
- Validação de caminhos para evitar path traversal
- Limite de tamanho de arquivos lidos (1MB)
- Tratamento de entradas malformadas
- Timeout em chamadas externas (10s)

## Limitações

- O dataset inicial é **demonstrativo** e pequeno (30 amostras)
- O modelo de ML é simples e não deve ser usado em produção
- A API não possui autenticação (não usar em produção sem adicionar)
- O scanner suporta apenas requirements.txt, package.json e pom.xml
- A API HTTP é simples e não utiliza bibliotecas avançadas

## Próximos passos

- [ ] Adicionar mais formatos de dependências (Gemfile, go.mod, Cargo.toml)
- [ ] Implementar cache de consultas OSV
- [ ] Adicionar autenticação na API
- [ ] Melhorar o modelo de ML com mais dados
- [ ] Adicionar suporte a mais ecossistemas
- [ ] Implementar análise de código estática
