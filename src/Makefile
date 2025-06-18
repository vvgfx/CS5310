# ImGui objects
IMGUI_OBJS = imgui.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui_impl_glfw.o imgui_impl_opengl3.o

# Original objects plus ImGui
OBJS = RenderingEngine.o View.o Controller.o Model.o GUIView.o GUIController.o $(IMGUI_OBJS)

INCLUDES = -I../include -I../include/Imgui -I../include/Imgui/backends
LIBS = -L../lib
LDFLAGS = -lglad -lglfw3
CFLAGS = -g -std=c++11
PROGRAM = Rendering_Engine

ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    LDFLAGS += -lopengl32 -lgdi32
    PROGRAM :=$(addsuffix .exe,$(PROGRAM))
	COMPILER = g++
else ifeq ($(shell uname -s),Darwin)     # is MACOSX
    LDFLAGS += -framework Cocoa -framework OpenGL -framework IOKit
	COMPILER = clang++
endif

Rendering_Engine: $(OBJS)
	$(COMPILER) -o $(PROGRAM) $(OBJS) $(LIBS) $(LDFLAGS)

RenderingEngine.o: RenderingEngine.cpp
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c RenderingEngine.cpp

View.o: View.cpp View.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c View.cpp  

Controller.o: Controller.cpp Controller.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c Controller.cpp

GUIController.o: GUIController.cpp GUIController.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c GUIController.cpp  

Model.o: Model.cpp Model.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c Model.cpp      

GUIView.o: GUIView.cpp GUIView.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c GUIView.cpp

# ImGui compilation rules
imgui.o: ../include/Imgui/imgui.cpp
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c ../include/Imgui/imgui.cpp

imgui_demo.o: ../include/Imgui/imgui_demo.cpp
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c ../include/Imgui/imgui_demo.cpp

imgui_draw.o: ../include/Imgui/imgui_draw.cpp
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c ../include/Imgui/imgui_draw.cpp

imgui_tables.o: ../include/Imgui/imgui_tables.cpp
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c ../include/Imgui/imgui_tables.cpp

imgui_widgets.o: ../include/Imgui/imgui_widgets.cpp
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c ../include/Imgui/imgui_widgets.cpp

imgui_impl_glfw.o: ../include/Imgui/backends/imgui_impl_glfw.cpp
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c ../include/Imgui/backends/imgui_impl_glfw.cpp

imgui_impl_opengl3.o: ../include/Imgui/backends/imgui_impl_opengl3.cpp
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c ../include/Imgui/backends/imgui_impl_opengl3.cpp

RM = rm -f
ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    RM := del
endif

clean:
	$(RM) $(OBJS) $(PROGRAM)