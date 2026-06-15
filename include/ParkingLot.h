#ifndef PARKINGMANAGER_PARKINGLOT_H
#define PARKINGMANAGER_PARKINGLOT_H

#include "ParkingSpot.h"
#include <string>
#include <vector>
#include <mutex>

/**
 * @brief Главный класс управления парковкой
 *
 * Содержит вектор парковочных мест, общую выручку,
 * почасовой тариф и мьютекс для синхронизации потоков.
 * Реализует многопоточный доступ с блокировками.
 */
class ParkingLot {
private:
    mutable std::mutex mtx; ///< Мьютекс для синхронизации потоков
    std::vector<ParkingSpot> Spots; ///< Вектор всех парковочных мест
    float revenue; ///< Общая выручка парковки (рубли)
    float hourlyRate; ///< Стоимость часа стоянки (рубли)
public:
    /**
     * @brief Конструктор парковки
     * @param path Путь к файлу конфигурации (.txt)
     * @param price Почасовой тариф (рубли)
     * @throws std::runtime_error Если файл не загружен или пуст
     *
     * Загружает карту мест из файла, инициализирует выручку = 0.
     * Формат файла: числа с плавающей точкой через пробел (размеры мест).
     */
    ParkingLot(const std::string& path, int price);

    /**
     * @brief Загрузить карту мест из файла
     * @param path Путь к файлу
     * @return true если файл успешно загружен и есть хотя бы одно место
     * @return false если файл не найден или пуст
     */
    bool LoadSpotsFromFile(const std::string& path);

    /**
     * @brief Обработка въезда машины
     * @param vehicleNumber Госномер машины
     * @param vehicleSize Размер ТС (1.0, 0.5, 2.5)
     * @param SpotId [выходной] Номер места, которое заняла машина
     * @return true если место найдено и машина припаркована
     * @return false если нет места или машина уже на парковке
     *
     * Алгоритм:
     * 1. Захватывает мьютекс (блокирует доступ других потоков)
     * 2. Проверяет, не припаркована ли уже машина
     * 3. Ищет первое свободное место подходящего размера
     * 4. Паркует машину и возвращает номер места
     */
    bool EnterVehicle(const std::string& vehicleNumber, float vehicleSize, int& SpotId);

    /**
     * @brief Обработка выезда машины
     * @param vehicleNumber Госномер машины
     * @param cost [выходной] Стоимость парковки (рубли)
     * @param totalRevenue [выходной] Общая выручка парковки после оплаты
     * @return true если машина найдена и выезд успешен
     * @return false если машина не найдена
     *
     * Алгоритм:
     * 1. Захватывает мьютекс
     * 2. Ищет машину по номеру
     * 3. Вызывает spot.leave() для получения времени стоянки
     * 4. Вычисляет стоимость: время * hourlyRate
     * 5. Обновляет выручку и возвращает значения
     */
    bool ExitVehicle(const std::string& vehicleNumber, float& cost, float& totalRevenue);

    /**
     * @brief Получить количество свободных мест
     * @return int Число свободных мест
     */
    int getFreeSpots() const;

    /**
     * @brief Получить текущую выручку
     * @return float Общая выручка в рублях
     */
    float getRevenue() const;

    /**
     * @brief Проверить, находится ли машина на парковке
     * @param vehicleNumber Госномер
     * @return true если машина на парковке, false если нет
     */
    bool isParked(const std::string& vehicleNumber) const;
};

#endif //PARKINGMANAGER_PARKINGLOT_H