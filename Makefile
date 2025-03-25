BUILD_DIR:=./build
OBJECT_DIR:=$(BUILD_DIR)/objects

all: objects

%.o: %.c
	gcc -c $< -o $(OBJECT_DIR)/$@

objects: $(OBJECT_DIR) sysinfo_device.o sysinfo_server.o main.o

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(OBJECT_DIR): $(BUILD_DIR)
	@mkdir -p $(OBJECT_DIR) 

clean:
	@rm -rf $(BUILD_DIR)