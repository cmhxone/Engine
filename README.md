# Engine

Make a `Vulkan` based 3D game-engine

### Dependencies

- `Vulkan`
- `SDL2`
- `Inih`
- `spdlog`

### Install dependencies with vcpkg

```
$> vcpkg install sdl2[vulkan] vulkan inih spdlog --triplet=x64-windows

$> vcpkg integrate install --triplet=x64-windows
```