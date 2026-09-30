.PHONY: configure build test clean

BUILD_DIR ?= build

configure:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release -DOFCL_BUILD_TESTS=ON -DOFCL_BUILD_EXAMPLES=ON

build: configure
	cmake --build $(BUILD_DIR) -j$$(nproc)

test: build
	cd $(BUILD_DIR) && ctest --output-on-failure

clean:
	rm -rf $(BUILD_DIR)
