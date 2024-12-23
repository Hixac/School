from abc import ABC, abstractmethod
from os.path import isfile
import json

class Vehicle(ABC):
    def __init__(self):
        self._brand = ""
        self._model = ""
        self._year = 0
        self._max_speed = 0
        self._weight = 0
        self._price = 0

    def init(self, brand, model, year, max_speed, weight, price):
        self._brand = brand
        self._model = model
        self._year = year
        self._max_speed = max_speed
        self._weight = weight
        self._price = price

    @abstractmethod
    def display_info(self):
        pass

    def input_CLI(self):
        self._brand = input("Введите бренд транспорта: ")
        self._model = input("Введите модель транспорта: ")
        self._data = input("Введите год изготовления транспорта: ")
        self._max_speed = input("Введите максимальная скорость транспорта: ")
        self._weight = input("Введите вес транспорта: ")
        self._price = input("Введите цену транспорта: ")

    def output(self):
        return {'brand': self._brand, 'model': self._model, 'data': self._data, 'max_speed': self._max_speed, 'weight': self._weight, 'price': self._price}
    
    def my_name(self):
        return f"I am a {self.__class__.__name__}"

class Car(Vehicle):
    def __init__(self):
        super().__init__()

    def init(self, brand, model, year, max_speed, weight, price, num_doors):
        super().init(brand, model, year, max_speed, weight, price)
        self.__num_doors = num_doors

    def display_info(self):
        return (f"Car: {self._brand} {self._model}, Year: {str(self._year)}, "
                f"Max Speed: {str(self._max_speed)} km/h, Weight: {str(self._weight)} kg, "
                f"Price: {str(self._price)}, Number of Doors: {str(self.__num_doors)}")

    def input_CLI(self):
        super().input_CLI()
        self.__num_doors = input("Введите число дверей: ")

    def output(self):
        d = super().output()
        d['type'] = 'Car'
        d['num_doors'] = self.__num_doors
        return d

class Motorbike(Vehicle):
    def __init__(self):
        super().__init__()

    def init(self, brand, model, year, max_speed, weight, price, engine_capacity):
        super().init(brand, model, year, max_speed, weight, price)
        self.__engine_capacity = engine_capacity

    def display_info(self):
        return (f"Car: {self._brand} {self._model}, Year: {str(self._year)}, "
                f"Max Speed: {str(self._max_speed)} km/h, Weight: {str(self._weight)} kg, "
                f"Price: {str(self._price)}, Number of Doors: {str(self.__engine_capacity)} cc")

    def input_CLI(self):
        super().input_CLI()
        self.__engine_capacity = input("Введите вместимость двигателя: ")

    def output(self):
        d = super().output()
        d['type'] = 'Motorbike'
        d['engine_capacity'] = self.__engine_capacity
        return d

class Garage:
    def __init__(self):
        self.vehicles = []
    
    def add_vehicle(self, vehicle):
        self.vehicles.append(vehicle)

    def display_all_vehicles(self):
        for vehicle in self.vehicles:
            print(vehicle.display_info())
            print(vehicle.my_name())
            print()

    def display_vehicles_by_class(self, vehicle_class):
        for vehicle in self.vehicles:
            if isinstance(vehicle, vehicle_class):
                print(vehicle.display_info())
                print(vehicle.my_name())
                print()

    def display_vehicles_by_speed_range(self, min_speed, max_speed):
        for vehicle in self.vehicles:
            if min_speed <= int(vehicle._max_speed) <= max_speed:
                print(vehicle.display_info())
                print(vehicle.my_name())
                print()

    def load_vehicles_from_file(self, file_path):
        with open(file_path, 'r') as f:
            data = json.load(f)
            self.vehicles.clear()
            for item in data:
                if item['type'] == 'Car':
                    vehicle = Car()
                    vehicle.init(item['brand'], item['model'], item['year'], item['max_speed'], item['weight'], item['price'], item['num_doors'])
                elif item['type'] == 'Motorbike':
                    vehicle = Motorbike()
                    vehicle.init(item['brand'], item['model'], item['year'], item['max_speed'], item['weight'], item['price'], item['engine_capacity'])
                self.add_vehicle(vehicle)

    def save_vehicles_to_file(self, file_path):
        data = []
        for vehicle in self.vehicles:
            data.append(vehicle.output())
        with open(file_path, 'w') as file:
            json.dump(data, file, indent=4)

if __name__ == "__main__":
    garage = Garage()
    
    while True:
        print("Выберите действие:")
        print("1. Вывести все транспортные средства")
        print("2. Вывести транспортные средства определенного класса")
        print("3. Вывести транспортные средства по диапазону скоростей")
        print("4. Сохранить данные в файл")
        print("5. Создать экземпляр транспорта")
        print("6. Загрузить из файла гараж")
        print("7. Выйти")

        choice = input("Введите номер действия: ")

        if choice == '1':
            garage.display_all_vehicles()
        elif choice == '2':
            class_name = input("Введите название класса (Car/Motorbike): ")
            if class_name == 'Car':
                garage.display_vehicles_by_class(Car)
            elif class_name == 'Motorbike':
                garage.display_vehicles_by_class(Motorbike)
            else:
                print("Неверное название класса")
        elif choice == '3':
            min_speed = int(input("Введите минимальную скорость: "))
            max_speed = int(input("Введите максимальную скорость: "))
            garage.display_vehicles_by_speed_range(min_speed, max_speed)
        elif choice == '4':
            file_path = input("Введите имя файла для сохранения: ")
            garage.save_vehicles_to_file(file_path)
        elif choice == '5':
            class_name = input("Введите название класса (Car/Motorbike): ")
            if class_name == 'Car':
                car = Car()
                car.input_CLI()

                garage.add_vehicle(car)

                print("Машина добавлена!")
            elif class_name == 'Motorbike':
                mbike = Motorbike()
                mbike.input_CLI()
                
                garage.add_vehicle(mbike)

                print("Мотоцикл добавлен!")
            else:
                print("Неверное название класса")
        elif choice == '6':
            file_path = input("Введите имя файла для загрузки: ")
            if isfile(file_path):
                garage.load_vehicles_from_file(file_path)
            else: print("Файла несуществует!")
        elif choice == '7':
            break
        else:
            print("Неверный выбор")
            
