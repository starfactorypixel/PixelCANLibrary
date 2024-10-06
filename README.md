# Using in your project

In `platformio.ini` add library to the `lib_deps`:
```
lib_deps = 
	# Pixel's CAN library
	https://github.com/starfactorypixel/PixelCANLibrary
```

If you need particular branch or tag, use this syntax:
```
lib_deps = 
	https://github.com/username/library_name#branch_name
	https://github.com/username/library_name#v2.0
```



# Update library in your project

Two ways:
1. Run command in the terminal
2. Create task and run it

## Run command in the terminal
Open terminal in the project's folder and run this command:
`pio lib update`

## Create task and run it
Add the following task "Update Libraries" to your project and run it with `ctrl+alt+P` → `Tasks: Run Task` → `Update Libraries` → `Continue without scanning the task output`.  
Task code (place it in the `.vscode\tasks.json`):
```
{
	"version": "2.0.0",
	"tasks": [
		{
			"type": "PlatformIO",
			"task": "Build",
			"problemMatcher": [
				"$platformio"
			],
			"group": "build",
			"label": "PlatformIO: Build"
		},
		{
			"type": "shell",
			"label": "Update Libraries",
			"command": "pio lib update",
			"presentation": {
				"echo": true,
				"reveal": "always",
				"focus": false,
				"panel": "shared",
				"showReuseMessage": true,
				"clear": false
			}
		}
	]
}
```