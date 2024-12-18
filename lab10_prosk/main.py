import tkinter as tk
from tkinter import messagebox, filedialog

def load_from_file():
    file_path = filedialog.askopenfilename(filetypes=[("Text files", "*.txt")])
    if file_path:
        with open(file_path, 'r') as file:
            data = file.read()
            numbers = list(map(int, data.split()))
            entry.delete(0, tk.END)
            entry.insert(0, ' '.join(map(str, numbers)))

def save_to_file(result):
    file_path = filedialog.asksaveasfilename(defaultextension=".txt", filetypes=[("Text files", "*.txt")])
    if file_path:
        with open(file_path, 'w') as file:
            file.write(' '.join(map(str, result)))

def process_numbers():
    try:
        numbers = list(map(int, entry.get().split()))
        if not numbers:
            raise ValueError("Список чисел пуст")

        # Вариант 1: Заменить каждый элемент списка A со значением 0 на сумму элементов списка B
        A = [x for x in numbers if x != 0]
        B = [x for x in numbers if x == 0]

        if not B:
            messagebox.showinfo("Результат", "В списке нет нулевых элементов")
            return

        sum_B = sum(B)
        result = [x if x != 0 else sum_B for x in numbers]

        messagebox.showinfo("Результат", ' '.join(map(str, result)))
        save_to_file(result)
    except ValueError as e:
        messagebox.showerror("Ошибка", str(e))

def main():
    global entry

    root = tk.Tk()
    root.title("Общее задание")

    frame = tk.Frame(root)
    frame.pack(padx=10, pady=10)

    tk.Label(frame, text="Введите список чисел:").grid(row=0, column=0, sticky=tk.W)
    entry = tk.Entry(frame, width=50)
    entry.grid(row=0, column=1, padx=5, pady=5)

    tk.Button(frame, text="Загрузить из файла", command=load_from_file).grid(row=1, column=0, columnspan=2, pady=5)
    tk.Button(frame, text="Выполнить", command=process_numbers).grid(row=2, column=0, columnspan=2, pady=5)

    root.mainloop()

if __name__ == "__main__":
    main()
