class Encryption:
    def __init__(self):
        self.key_char = ['1', '2', '3', '4', '5', '6', '7',
                         '8', '9']
        self.message = ""
        self.key = ""

    def input_data(self):
        # Просимо користувача ввести повідомлення та новий ключ
        self.message = input("Введіть повідомлення: ")
        self.key = input("Введіть новий ключ: ")

    def encryption(self, message):
        result_message = ""
        for i in range(len(self.key)):
            # Для кожного символу ключа обчислюємо зміщення та додаємо відповідні символи повідомлення до результату
            for j in range(self.key.index(self.key_char[i]), len(message), len(self.key)):
                result_message += message[j]
        return result_message

    def run(self):
        self.input_data()
        # Зашифруємо повідомлення та виведемо результат
        encrypted_message = self.encryption(self.message)
        print("Закодоване повідомлення:", encrypted_message)


def main():
    # Створимо екземпляр класу шифрування та запустимо програму
    enc = Encryption()
    enc.run()


if __name__ == "__main__":
    main()