A Simple ROT13 server showcasing Telnet++ and Server++.

To build: you will need the Conan remotes that are registered with the following commands:

```
conan remote add kazdragon-conan https://api.bintray.com/conan/kazdragon/conan-public
```

In addition, because Conan Center (currently) takes a while to work out where the Boost packages are, the following line is useful in order to make the build more robust:

```
conan config set general.request_timeout=120
```

Conan packages are then installed as following:

```
conan install -s compiler.libcxx=libstdc++11 -s cppstd=14 --build=missing .
```

You can then build with:

```
cmake .
make -j
```

Running the `rot13server` application will then bind to an available port, to which you can connect your Telnet session.