# dnscache

## Сложность операций:

### 1. Вставка записей (update):
- В среднем случае: O(1) для поиска в unordered_map и перемещения элемента в список.
- В худшем случае (при достижении лимита): O(maxSize) из-за удаления старейшей записи.

### 2. Обновление существующих записей:
- O(1) для поиска в unordered_map и обновления значения.

### 3. Поиск записей (resolve):
- O(1) для поиска в unordered_map и перемещения элемента в список.

## Дополнительные пояснения:

1. Используется std::list для хранения порядка использования записей, что позволяет эффективно удалять старейшие записи при достижении лимита.
2. std::unordered_map используется для быстрого доступа к записям по имени.
3. Мьютекс (std::mutex) обеспечивает потокобезопасность при одновременном доступе нескольких потоков.
4. Реализован паттерн Singleton через класс DNSCacheSingleton, который гарантирует создание только одного экземпляра DNSCache в процессе.