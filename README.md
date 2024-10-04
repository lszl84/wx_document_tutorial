# wx_document_tutorial

Document/View Framework in wxWidgets.

[![Image](/screen.png)](https://www.youtube.com/watch?v=IGmIOIsRpD0)

Full Tutorial: [https://www.youtube.com/watch?v=IGmIOIsRpD0](https://www.youtube.com/watch?v=IGmIOIsRpD0). Learn how to use `wxDocument`, `wxView`, `wxDocManager`, `wxDocTemplate` and more!

### Building

To build the project, use:

```bash
cmake -S. -Bbuild
cmake --build build
```

This will create a directory named `build` and create all build artifacts there. The main executable can be found in the `build/subprojects/Build/wx_doocument_tutorial_core` folder.

### Reqiurements

If you don't have wxWidgets installed, the CMake script will download and build the dependencies automatically.

But if CMake script finds wxWidgets on your system, make sure it's at least version 3.1.0.

---
Check out the blog for more! [www.lukesdevtutorials.com](https://www.lukesdevtutorials.com)
---
