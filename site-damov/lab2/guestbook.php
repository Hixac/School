<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <title>Гостевая книга</title>
</head>
<body>
    <h1>Гостевая книга</h1>
    <form action="save_message.php" method="post">
        <label for="name">Имя:</label>
        <input type="text" id="name" name="name" required><br><br>
        <label for="message">Сообщение:</label>
        <textarea id="message" name="message" required></textarea><br><br>
        <input type="submit" value="Отправить">
    </form>
    <h2>Сообщения:</h2>
    <div id="messages">
        <?php include 'messages.php'; ?>
    </div>
</body>
</html>
