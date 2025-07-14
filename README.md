<p align="center">
	<img src="assets/spark.svg" width="512px">
</p>

<p align="center">A lightweight Linux alias manager.</p>

## Description
A little project of mine I started to manage my own aliases, but might help others manage their aliases too :)

I know the code might not be of the highest quality, but you know, it's all a learning experience!

### Features

- Simple and understandable command structure
- Aliases stored in a JSON format
- No unnecessary fluff

## Getting Started
### Requirements

- Operating system: Linux (might also work on Windows or macOS, but you're on your own there)
- Compiler: gcc (or any other compiler with C99 support)

### Dependencies
As of now (the planning stage and early development), there are no dependencies you need to have installed to compile.<br>
The dependencies necessary are already included in the source code.

### Installation / Building
I'm not providing any executables or distro-specific packages, so you need to compile it yourself.<br><br>

To compile, use the provided bash script:
```
$ ./make.sh
```
And make sure it's executable:
```
$ chmod +x make.sh
```
After that, move the executable to anywhere in your PATH to access it from anywhere.

## Usage

Executing
```
$ spark
```
in the terminal will print the help message and quit.<br><br>

Defining a new alias is easy, just run the following command:
```
$ spark --create <name> <command>
```
where `<name>` is the name you picked for your alias, and `<command>` is, as the name suggests, the command you want to be executed.<br><br>

Executing an alias is really easy as well, just run:
```
$ spark <name>
```
where `<name>` is the name of the alias you want to execute. Simple as that.

And as for removing an alias, it's analogous to creating one, but omitting the `<command>` parameter:
```
$ spark --remove <name>
```
where `<name>` is the name of the alias you want to remove.

## Maintainers

- tomeczeklmaooo

## Acknowledgements

- [tidwall](https://github.com/tidwall) for his JSON parser: [json.c](https://github.com/tidwall/json.c) (MIT License)

## License
This project is licensed under the BSD-3-Clause License - see the <code>LICENSE</code> file for details.