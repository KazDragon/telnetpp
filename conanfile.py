from conans import ConanFile
from conans import CMake

class ConanTelnetpp(ConanFile):
    name = "telnetpp"
    version = "2.0.1"
    url = "https://github.com/KazDragon/telnetpp"
    author = "KazDragon"
    license = "MIT"
    settings = "os", "arch", "compiler", "build_type"
    generators = "cmake"
    exports = "*.hpp", "*.in", "*.cpp", "CMakeLists.txt", "*.md", "LICENSE"
    description = "A library for handling the Telnet protocol"
    requires = ("boost_container/[>=1.69]@bincrafters/stable",
                "boost_exception/[>=1.69]@bincrafters/stable",
                "boost_signals2/[>=1.69]@bincrafters/stable",
                "boost_variant/[>=1.69]@bincrafters/stable",
                "gsl-lite/[>=0.34]@nonstd-lite/stable")
    build_requires = ("gtest/[>=1.8.1]@bincrafters/stable")
    options = {"shared": [True, False], "withZlib": [True, False], "coverage": [True, False], "sanitize" : ["off", "address"]}
    default_options = {"shared": False, "withZlib": True, "coverage": False, "sanitize": "off"}

    def requirements(self):
        if (self.options.withZlib):
            self.requires("zlib/[>=1.2.11]@conan/stable")

    def configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_SHARED_LIBS"] = self.options.shared
        cmake.definitions["TELNETPP_WITH_ZLIB"] = self.options.withZlib
        cmake.definitions["TELNETPP_COVERAGE"] = self.options.coverage
        cmake.definitions["TELNETPP_SANITIZE"] = self.options.sanitize
        cmake.configure()
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="include")
        self.copy("*.lib", dst="lib", src="lib", keep_path=False)
        self.copy("*.dll", dst="bin", src="bin", keep_path=False)
        self.copy("*.dylib", dst="bin", src="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.so.*", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["telnetpp"]
