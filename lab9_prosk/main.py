from abc import ABC, abstractmethod
from os.path import isfile
import json

class Vehicle(ABC):
    def __init__(self, brand, model, year, max_speed, weight, price):
        self._brand = brand
        self._model = model
        self._year = year
        self._max_speed = max_speed
        self._weight = weight
        self._price = price

    @abstractmethod
    def display_info(self):
        pass

    def my_name(self):
        return f"I am a {self.__class__.__name__}"

class Car(Vehicle):
    def __init__(self, brand, model, year, max_speed, weight, price, num_doors):
        super().__init__(brand, model, year, max_speed, weight, price)
        self.__num_doors = num_doors

    def display_info(self):
        return (f"Car: {self._brand} {self._model}, Year: {self._year}, "
                f"Max Speed: {self._max_speed} km/h, Weight: {self._weight} kg, "
                f"Price: {self._price}, Number of Doors: {self.__num_doors}")

class Motorbike(Vehicle):
    def __init__(self, brand, model, year, max_speed, weight, price, engine_capacity):
        super().__init__(brand, model, year, max_speed, weight, price)
        self.__engine_capacity = engine_capacity

    def display_info(self):
        return (f"Motorbike: {self._brand} {self._model}, Year: {self._year}, "
                f"Max Speed: {self._max_speed} km/h, Weight: {self._weight} kg, "
                f"Price: {self._price}, Engine Capacity: {self.__engine_capacity} cc")

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
                    vehicle = Car(item['brand'], item['model'], item['year'], item['max_speed'], item['weight'], item['price'], item['num_doors'])
                elif item['type'] == 'Motorbike':
                    vehicle = Motorbike(item['brand'], item['model'], item['year'], item['max_speed'], item['weight'], item['price'], item['engine_capacity'])
                self.add_vehicle(vehicle)

    def save_vehicles_to_file(self, file_path):
        data = []
        for vehicle in self.vehicles:
            if isinstance(vehicle, Car):
                data.append({
                    'type': 'Car',
                    'brand': vehicle._brand,
                    'model': vehicle._model,
                    'year': vehicle._year,
                    'max_speed': vehicle._max_speed,
                    'weight': vehicle._weight,
                    'price': vehicle._price,
                    'num_doors': vehicle._Car__num_doors
                })
            elif isinstance(vehicle, Motorbike):
                data.append({
                    'type': 'Motorbike',
                    'brand': vehicle._brand,
                    'model': vehicle._model,
                    'year': vehicle._year,
                    'max_speed': vehicle._max_speed,
                    'weight': vehicle._weight,
                    'price': vehicle._price,
                    'engine_capacity': vehicle._Motorbike__engine_capacity
                })
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
                brand = input("Введите бренд машины: ")
                model = input("Введите модель машины: ")
                data = input("Введите год изготовления машины: ")
                max_speed = input("Введите максимальная скорость машины: ")
                weight = input("Введите вес машины: ")
                price = input("Введите цену машины: ")
                num_doors = input("Введите количество дверей машины: ")
                car = Car(brand, model, data, max_speed, weight, price, num_doors)

                garage.add_vehicle(car)

                print("Машина добавлена!")
            elif class_name == 'Motorbike':
                brand = input("Введите бренд мотоцикла: ")
                model = input("Введите модель мотоцикла: ")
                data = input("Введите год изготовления мотоцикла: ")
                max_speed = input("Введите максимальная скорость мотоцикла: ")
                weight = input("Введите вес мотоцикла: ")
                price = input("Введите цену мотоцикла: ")
                engine_capacity = input("Введите вместимость двигателя мотоцикла: ")
                
                mbike = Motorbike(brand, model, data, max_speed, weight, price, engine_capacity)

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
            
