import tkinter as tk
from tkinter import messagebox, filedialog

def load_from_file():
    file_path = filedialog.askopenfilename(filetypes=[("Text files", "*.txt")])
    if file_path:
        with open(file_path, 'r') as file:
            data1 = file.readline().strip()
            data2 = file.readline().strip()

            entryA.delete(0, tk.END)
            entryA.insert(0, data1)
            entryB.delete(0, tk.END)
            entryB.insert(0, data2)

def save_to_file(a, b):
    file_path = filedialog.asksaveasfilename(defaultextension=".txt", filetypes=[("Text files", "*.txt")])
    if file_path:
        with open(file_path, 'w') as file:
            file.write(' '.join(map(str, a)))
            file.write('\n')
            file.write(' '.join(map(str, b)))
            file.write('\n')

def process_numbers():
    a = entryA.get().split()
    b = entryB.get().split()
    final = []

    for i in range(len(a)):
        if a[i] == '0':
            final.append(sum(map(int, b)))
            continue
        final.append(a[i])
            
    messagebox.showinfo("Результат", ' '.join(map(str, final)))
    save_to_file(a, b)

def main():
    global entryA
    global entryB

    root = tk.Tk()
    root.title("Наилабораторнейшая")

    frame = tk.Frame(root)
    frame.pack(padx=10, pady=10)

    tk.Label(frame, text="Введите список чисел").grid(row=0, column=1)
    tk.Label(frame, text="A").grid(row=1, column=0)
    tk.Label(frame, text="B").grid(row=2, column=0)
    entryA = tk.Entry(frame, width=50)
    entryA.grid(row=1, column=1, padx=5, pady=5)
    entryB = tk.Entry(frame, width=50)
    entryB.grid(row=2, column=1, padx=5, pady=5)

    tk.Button(frame, text="Загрузить из файла", command=load_from_file).grid(row=3, column=0, columnspan=2, pady=5)
    tk.Button(frame, text="Выполнить", command=process_numbers).grid(row=4, column=0, columnspan=2, pady=5)

    root.mainloop()

if __name__ == "__main__":
    main()
