![Screenshot](https://raw.githubusercontent.com/PkXwmpgN/urho3d-board-replication/master/screenshots/screen.png)

### Компоненты
1. **TouchDispatcher**. Создается в корневом узле сцене на стороне клиента.
Отслеживает клики мышкой по объектам, отсылает уведомления E_TOUCHOBJECT соответствующим узлам сцены.

2. **TouchClient**. Создается в узлах, по которым будут собираться клики (ячейки доски)
Подписывается на уведомление E_TOUCHOBJECT от TouchDispatcher, отсылает идентификатор узла на сервер (в Controls)

3. **TouchServer**. Создается в корневом узле реплицированной сцены на стороне сервера.
Собирает информацию о кликах, получает идентификаторы объектов, по которым были клики на стороне клиента.
Отсылает уведомление E_TOUCHREACTION соответствующим узлам сцены. Приложение подписывается на уведомление E_TOUCHREACTION, определяя реакцию на клик (раскрашиваем ячейки цветом клиента)

3. **BoardState**. Компонет узла на стороне сервера, хранит состояние узла занят/свободен

4. **BoardCamera**. Компонент описывает перемещение камеры (реализация из примеров)

### Сервер
1. Создает реплицированную сцену
2. Создает компонент TouchServer и подписывается на E_TOUCHREACTION
3. Раскрашивает свободные ячейки доски
4. Сцена синхронизируется с клиентами

### Клиент
1. Создает компонент TouchDispatcher
2. Компонент TouchClient ячейки отсылает клик на сервер со своим идентификатором

Сборка с опцией URHO3D_C++11.
Собранное приложение bin/Board (Ubuntu)
