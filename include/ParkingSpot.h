#ifndef PARKINGMANAGER_PARKINGSPOT_H
#define PARKINGMANAGER_PARKINGSPOT_H

#include <stdexcept>
#include <string>

/**
 * @brief Класс, представляющий одно парковочное место
 *
 * Хранит информацию о размере места, его занятости,
 * госномере припаркованной машины и времени въезда.
 * Используется классом ParkingLot для управления парковкой.
 */
class ParkingSpot {
 private:
  int id;         ///< Уникальный идентификатор места (начинается с 1)
  float size;     ///< Размер места (1.0 - легковое, 2.5 - грузовое, 0.5 - мото)
  bool occupied;  ///< Флаг занятости (true - занято, false - свободно)
  std::string
      VehicleNumber;  ///< Госномер припаркованного ТС (пусто, если свободно)
  std::time_t
      entryTime;  ///< Время въезда в формате time_t (для расчёта стоимости)
 public:
  /**
   * @brief Конструктор парковочного места
   * @param entery_id Номер места (уникальный)
   * @param entery_size Размер места в условных единицах
   */
  ParkingSpot(int entery_id, float entery_size);
  /**
   * @brief Запарковать машину на место
   * @param vehiclenumber Госномер машины
   * @return true если место свободно и парковка успешна
   * @return false если место уже занято
   */
  bool park(const std::string& vehiclenumber);
  /**
   * @brief Освободить место и получить время стоянки
   * @return Время стоянки в часах (округлённое вверх до целого)
   * @throws std::logic_error Если место уже свободно (вызвано по ошибке)
   *
   * Алгоритм:
   * 1. Проверяет, что место занято
   * 2. Вычисляет разницу между текущим временем и временем въезда
   * 3. Переводит секунды в часы (деление на 60 — каждая минута считается за
   * час)
   * 4. Округляет вверх с помощью std::ceil
   * 5. Освобождает место и очищает номер
   */
  double leave();
  /**
   * @brief Получить номер места
   * @return int Номер места
   */
  int getId() const;
  /**
   * @brief Получить размер места
   * @return float Размер в условных единицах
   */
  float getSize() const;
  /**
   * @brief Проверить, занято ли место
   * @return true если занято, false если свободно
   */
  bool isOccupied() const;
  /**
   * @brief Получить госномер припаркованной машины
   * @return const std::string& Ссылка на строку с номером (пусто, если
   * свободно)
   */
  const std::string& getVehicleNumber() const;
};

#endif  // PARKINGMANAGER_PARKINGSPOT_H