# Presentación:
**Presentado por / presented by:** Juan Mateo Rozo Torres - jrozot

# 🛠 Buscador de Productos Alimenticios — OpenFoodFacts

## 📘 Descripción del Proyecto

Este es un software sin nombre definido que funciona como un **motor de búsqueda simple** para un archivo `.csv` proveniente de [Open Food Facts](https://world.openfoodfacts.org/).

Ha sido desarrollado como un ejercicio académico para el curso de **Sistemas Operativos 2025-1** en la **Universidad Nacional de Colombia**.

El propósito de este proyecto es **desarrollar habilidades de programación** utilizando un dataset extenso y estructurado como el de Open Food Facts. Por ello, los criterios de búsqueda han sido seleccionados con base en su **valor didáctico**, priorizando la implementación de estructuras de datos, filtrado y organización lógica del código, más allá de la utilidad práctica inmediata.

---

## ▶️ Cómo usar el programa

1. Compilar el proyecto utilizando el `Makefile` incluido.
2. Ejecutar el módulo `search_module` **primero**, en una consola separada.
   - Este módulo debe ser detenido manualmente con la señal `SIGINT` (`Ctrl + C`).
3. Ejecutar el módulo `user_interface <server_ip>` en otra terminal.
   - Puede terminarse escribiendo `exit`.
4. Asegúrese de que el archivo `.csv` esté ubicado como:

./Data/products.csv


5. Si los siguientes archivos no existen, el programa los generará automáticamente:

./Data/code_hash.dat
./Data/main_index.dat


6. El sistema luego le pedirá ingresar un **código EAN** y mostrará la información del producto correspondiente.

---

## 📚 Acerca de OpenFoodFacts

Este proyecto utiliza datos del sitio colaborativo **Open Food Facts**, una base de datos libre y abierta sobre productos alimenticios de todo el mundo.

- Sitio web: [https://world.openfoodfacts.org/](https://world.openfoodfacts.org/)
- Licencia de datos: [Open Database License (ODbL)](https://opendatacommons.org/licenses/odbl/)
- Más información: [https://world.openfoodfacts.org/data](https://world.openfoodfacts.org/data)

¡Gracias a la comunidad de OpenFoodFacts por hacer posible este proyecto educativo!

---

## 🤖 Uso de inteligencia artificial

Este proyecto ha sido **impulsado por inteligencia artificial**, con la ayuda directa de **ChatGPT 4o** (OpenAI). La mayoría del código, documentación y estructura fueron generados, adaptados o sugeridos por esta herramienta, con supervisión y edición humana.

---

# 🛠 Food Product Search Tool — OpenFoodFacts

## 📘 Project Description

This is an unnamed piece of software that works as a **simple search engine** for a `.csv` file from [Open Food Facts](https://world.openfoodfacts.org/).

It has been developed as an academic exercise for the **Operating Systems 2025-1** course at the **National University of Colombia**.

The purpose of this project is to **develop programming skills** using a large and structured dataset like Open Food Facts. Therefore, the search criteria have been chosen based on their **didactic value**, prioritizing the implementation of data structures, filtering logic, and clean code organization over immediate practical use.

---

## ▶️ How to Use

1. Compile the project using the provided `Makefile`.
2. Run the `search_module` **first**, in a separate terminal.
- This module must be stopped manually with a `SIGINT` signal (`Ctrl + C`).
3. Then run the `user_interface` module.
- This module can be exited by typing `exit`.
4. Ensure the `.csv` file is located at:

./Data/products.csv


5. If the following files are missing, the program will generate them:

./Data/code_hash.dat
./Data/main_index.dat


6. The system will then prompt for an **EAN code** and display information about the matching product.

---

## 📚 About OpenFoodFacts

This project makes use of data from **Open Food Facts**, a free and open database of food products from around the world.

- Website: [https://world.openfoodfacts.org/](https://world.openfoodfacts.org/)
- Data License: [Open Database License (ODbL)](https://opendatacommons.org/licenses/odbl/)
- More info: [https://world.openfoodfacts.org/data](https://world.openfoodfacts.org/data)

We thank the OpenFoodFacts community for making this educational project possible!

---

## 🤖 Use of AI

This project was **AI-assisted**, with direct support from **ChatGPT 4o** (OpenAI). Most of the code, documentation, and structural ideas were generated, adapted, or improved by this tool, under human supervision and final editing.
