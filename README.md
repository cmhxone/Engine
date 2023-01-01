# Engine

Make a `Vulkan` based 3D game-engine

### Dependencies

- `Vulkan`
- `SDL3`
- `Inih`
- `spdlog`

### Install dependencies with vcpkg

```
$> vcpkg install vulkan inih spdlog --triplet=x64-windows

$> vcpkg integrate install --triplet=x64-windows
```

### Build SDL3 manually

```
$> git clone https://github.com/libsdl-org/SDL
```

then just read and follow `INSTALL.txt` file