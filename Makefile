BUILD_DIR:=./build
OBJECT_DIR:=$(BUILD_DIR)/objects
SRCS:=sysinfo_device.c sysinfo_server.c main.c request_utils.c
OBJS:=$(SRCS:%.c=%.o)
OUTPUTTED_OBJS:=$(OBJS:%.o=$(OBJECT_DIR)/%.o)

all: objects
	gcc -o $(BUILD_DIR)/sysinfo_server $(OUTPUTTED_OBJS)

%.o: %.c
	gcc -c $< -o $(OBJECT_DIR)/$@

objects: $(OBJECT_DIR) $(OBJS)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(OBJECT_DIR): $(BUILD_DIR)
	@mkdir -p $(OBJECT_DIR) 

clean:
	@rm -rf $(BUILD_DIR)