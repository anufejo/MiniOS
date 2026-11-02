# MiniOS – A Simple Educational Operating System

## 1. Project Overview

MiniOS is a beginner-friendly educational operating system developed using C and Assembly. It demonstrates the basic booting process, kernel initialization, and display of system information.

It uses GRUB as its bootloader and QEMU to run the operating system in a virtual machine.

## 2. Technologies Used

- C – Kernel implementation
- Assembly – Kernel entry point and stack initialization
- GRUB – Bootloader
- LLVM Clang – Compilation
- LLD – Linking
- QEMU – Operating system emulation
- Git and GitHub – Version control and collaboration

## 3. Project Structure

```text
MiniOS/
│
├── boot/
│
├── kernel/
│   ├── boot.s
│   ├── kernel.c
│   │
│   └── include/
│       └── kernel.h
│
├── iso/
│   └── boot/
│       ├── kernel.elf
│       └── grub/
│           └── grub.cfg
│
├── linker.ld
├── README.md
└── .gitignore
```

## 4. How MiniOS Boots

1. The computer starts the GRUB bootloader.
2. GRUB identifies the Multiboot-compatible kernel.
3. GRUB loads the kernel into memory.
4. The assembly entry point `_start` begins execution.
5. The stack is initialized.
6. Assembly calls `kernel_main()` from `kernel.c`.
7. The kernel displays information using VGA text memory.
8. The CPU enters a halt loop after initialization.

## 5. Current Features

- GRUB-based booting
- Multiboot-compatible kernel
- Assembly kernel entry point
- Stack initialization
- VGA text-mode output
- Kernel initialization message
- Basic system information display

## 6. Build Requirements

On macOS with Homebrew, install:

```bash
brew install llvm lld xorriso i686-elf-grub qemu
```

## 7. Build Instructions

Run these commands from the MiniOS project directory.

Compile the assembly file:

```bash
/opt/homebrew/opt/llvm/bin/clang -target i686-elf -m32 -c kernel/boot.s -o boot.o
```

Compile the C file:

```bash
/opt/homebrew/opt/llvm/bin/clang -target i686-elf -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -c kernel/kernel.c -o kernel.o
```

Link the kernel:

```bash
ld.lld -m elf_i386 -T linker.ld -nostdlib boot.o kernel.o -o kernel.elf
```

Verify Multiboot compatibility:

```bash
i686-elf-grub-file --is-x86-multiboot kernel.elf
```

Copy the kernel into the ISO directory:

```bash
cp kernel.elf iso/boot/kernel.elf
```

Create the bootable ISO:

```bash
i686-elf-grub-mkrescue -o minios.iso iso
```

Run MiniOS using QEMU:

```bash
qemu-system-i386 -cdrom minios.iso -m 128M -display cocoa,full-screen=off
```

## 8. Current Limitations

This is an initial educational kernel prototype. It currently does not implement keyboard input, multitasking, memory management, or a filesystem.

These features are planned as separate modules.

## 9. Future Enhancements

- Keyboard input handling
- Command-line interface
- Basic shell commands
- Calculator functionality
- Memory management
- Process management

## 10. Team Development

MiniOS is being developed by a three-member team. The kernel and bootloader form the foundation, while additional modules are developed separately and integrated into the final system.
