from conan import ConanFile

class Recipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps", "VirtualRunEnv"

    def requirements(self):
        self.requires("libevent/[>=2.0]")

    def build_requirements(self):
        self.tool_requires("cmake/[>=4.0]")
        self.tool_requires("ninja/[>=1.13]")
