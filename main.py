import tkinter as tk
from tkinter import filedialog, messagebox
from tkinter import ttk
from typing import List, Optional, NamedTuple
import io
from cffi import FFI
from sys import platform

class Instruction(NamedTuple):
    label: Optional[str]
    operation: str
    op1: Optional[str]
    op2: Optional[str]

def parse_instructions(file_content: str) -> List[Instruction]:
    instructions = []
    stream = io.StringIO(file_content)

    for line in stream:
        line = line.strip()
        if not line:
            continue  # Ignora linhas vazias

        words = line.split()
        word1 = words[0]
        word2 = words[1] if len(words) > 1 else ""

        if word2 == "NULL":
            instructions.append(Instruction(label=word1, operation="", op1=None, op2=None))
        else:
            op1 = None
            op2 = None

            comma_pos = word2.find(',')
            if comma_pos != -1:
                op1 = word2[:comma_pos].strip()
                op2 = word2[comma_pos + 1:].strip()
            elif word2:
                op1 = word2.strip()

            instructions.append(Instruction(label=None, operation=word1, op1=op1, op2=op2))

    return instructions



ffi = FFI()

ffi.cdef("""
    void initVM(char* file_path);
    void executeVM(int(*readFn)(), void(*writeFn)(int));
    int getVMSp();
    int getVMIp();
    int getVMStack(int sp);
""")

if platform == "linux" or platform == "linux2":
    lib = ffi.dlopen("./vm.so")
elif platform == "win32":
    lib = ffi.dlopen("vm.dll")


def init_vm(file_path):
    lib.initVM(file_path.encode('utf-8'))

def execute_vm(read_fn, write_fn):
    @ffi.callback("int()")
    def read_cb():
        return read_fn()

    @ffi.callback("void(int)")
    def write_cb(value):
        write_fn(value)

    lib.executeVM(read_cb, write_cb)

def get_vm_sp():
    return lib.getVMSp()

def get_vm_ip():
    return lib.getVMIp()

def get_vm_stack(sp):
    return lib.getVMStack(sp)

class Console(tk.Frame):
    def __init__(self, parent=None, **kwargs):
        super().__init__(parent, **kwargs)
        self.ttytext = tk.Text(self, wrap=tk.WORD)
        self.ttytext.pack(fill=tk.BOTH, expand=True)
        self.ttytext.bind("<Return>", self.enter)
        self.line_start = "1.0"
        self.input_buffer = ""
        self.input_ready = False

    def enter(self, event):
        user_input = self.ttytext.get(self.line_start, tk.END).strip()
        self.input_buffer = user_input
        self.input_ready = True
        # self.ttytext.insert(tk.END, f"\nYou entered: {user_input}\n")
        self.ttytext.insert(tk.END, f"\n")
        self.ttytext.mark_set("insert", tk.END)
        self.ttytext.see(tk.END)
        self.line_start = self.ttytext.index(tk.INSERT)

        return 'break'

    def get_input(self):
        while not self.input_ready:
            self.update_idletasks()
            self.update()
        self.input_ready = False
        return self.input_buffer

    def print_output(self, text):
        self.ttytext.insert(tk.END, f"{text}")
        self.ttytext.mark_set("insert", tk.END)
        self.ttytext.see(tk.END)
        self.line_start = self.ttytext.index(tk.INSERT)

class VirtualMachineInterface:
    def __init__(self, root):
        self.root = root
        self.root.title("Virtual Machine Interface")

        self.instructions = []

        # Import bytecode button
        self.import_button = tk.Button(root, text="Import Bytecode", command=self.import_bytecode)
        self.import_button.grid(row=0, column=0, padx=10, pady=10)

        # Execute button
        self.execute_button = tk.Button(root, text="Execute", command=self.execute)
        self.execute_button.grid(row=0, column=1, padx=10, pady=10)

        # Instructions table
        self.instructions_label = tk.Label(root, text="Instructions")
        self.instructions_label.grid(row=1, column=0, padx=10, pady=5)
        self.instructions_table = ttk.Treeview(root, columns=("Address", "Label", "Instruction", "Operator1", "Operator2"), show="headings")
        self.instructions_table.heading("Address", text="Address")
        self.instructions_table.heading("Label", text="Label")
        self.instructions_table.heading("Instruction", text="Instruction")
        self.instructions_table.heading("Operator1", text="Operator1")
        self.instructions_table.heading("Operator2", text="Operator2")
        self.instructions_table.grid(row=2, column=0, columnspan=2, padx=10, pady=5)

        # Stack table
        self.stack_label = tk.Label(root, text="Stack")
        self.stack_label.grid(row=3, column=0, padx=10, pady=5)
        self.stack_table = ttk.Treeview(root, columns=("Address", "Value"), show="headings")
        self.stack_table.heading("Address", text="Address")
        self.stack_table.heading("Value", text="Value")
        self.stack_table.grid(row=4, column=0, columnspan=2, padx=10, pady=5)

        # Console for STDIN/STDOUT
        self.console = Console(self.root)
        self.console.grid(row=5, column=0, columnspan=2, padx=10, pady=10)

    def import_bytecode(self):
        file_path = filedialog.askopenfilename()
        self.file_path = file_path
        if file_path:
            file_content = open(file_path).read()
            self.instructions = parse_instructions(file_content)
            self.update_instructions_table()

    def update_instructions_table(self):
        for i in self.instructions_table.get_children():
            self.instructions_table.delete(i)

        for i,instr in enumerate(self.instructions):
            self.instructions_table.insert("", "end", values=(
                i,
                instr.label if instr.label else "",
                instr.operation,
                instr.op1 if instr.op1 else "",
                instr.op2 if instr.op2 else ""
            ))

    def select_instruction_pointer(self):
        ip = get_vm_ip()
        ip = min(len(self.instructions) - 1, ip)
        childs = self.instructions_table.get_children()
        self.instructions_table.selection_set([childs[ip]])
        self.instructions_table.see([childs[ip]])

    # update stack and instructions table in breakpoint
    def update_tables(self):
        self.select_instruction_pointer()

        for i in self.stack_table.get_children():
            self.stack_table.delete(i)

        sp = get_vm_sp()
        for i in range(sp):
            value = get_vm_stack(i)
            self.stack_table.insert("", "end", values=(i, value))

    def read_fn(self):
        self.update_tables()
        self.console.print_output("> ")
        while True:
            input_value = self.console.get_input()
            try:
                return int(input_value)
            except ValueError:
                self.console.print_output("Invalid input, please enter an integer.")

    def print_fn(self, var):
        self.console.print_output(f"Output: {var}\n")
        self.update_tables()

    def execute(self):
        if not self.file_path:
            return
        init_vm(self.file_path)
        execute_vm(self.read_fn, self.print_fn)
        self.update_tables()

# Create the main window
root = tk.Tk()
app = VirtualMachineInterface(root)
root.mainloop()






