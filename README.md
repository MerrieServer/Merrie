# Yes, very readme, thank you

# Dependencies
- boost (system, log, asio, beast)
- nlohmann-json 
- protobuf
- qt5
- openssl (optional)
- gtest (optional, if MERRIE_DO_UNIT_TESTS is on)

### Installing with vcpkg
```vcpkg install --triplet x64-windows-static boost boost-beast nlohmann-json openssl qt5 protobuf gtest```

### Code style
Code style and inspections for CLion are available for importing in code-style.xml and inspections.xml