<?php

$host = 'localhost';
$db = 'guestbook';
$user = 'root';
$password = 'lol';

try {
    $pdo = new PDO("mysql:host=$host;dbname=$db;charset=utf8", $user, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
    die("Ошибка подключения к базе данных: " . $e->getMessage());
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
   
    $name = htmlspecialchars(trim($_POST['name']));
    $message = htmlspecialchars(trim($_POST['message']));
    $date = date('Y-m-d H:i:s');

   
    if (!empty($name) && !empty($message)) {
        try {
            $stmt = $pdo->prepare("INSERT INTO entries (name, message, date) VALUES (:name, :message, :date)");
            $stmt->execute(['name' => $name, 'message' => $message, 'date' => $date]);
            $success = "Ваше сообщение успешно доставлено!";
        } catch (PDOException $e) {
            $error = "Ошибка сохранения сообщения" . $e->getMessage();
        }
    } else {
        $error = "Пожалуйста, заполните все поля!";
    }
}

try {
    $stmt = $pdo->query("SELECT name, message, date FROM entries ORDER BY date DESC");
    $entries = $stmt->fetchAll(PDO::FETCH_ASSOC);
} catch (PDOException $e) {
    die("Ошибка чтения данных: " . $e->getMessage());
}
?>

<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ð“Ð¾ÑÑ‚ÐµÐ²Ð°Ñ ÐºÐ½Ð¸Ð³Ð°</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .form-container { margin-bottom: 20px; }
        .messages { margin-top: 20px; border-top: 1px solid #ccc; padding-top: 10px; }
        .message { margin-bottom: 15px; }
        .message h4 { margin: 0; }
        .message p { margin: 5px 0; }
        .error { color: red; }
        .success { color: green; }
    </style>
</head>
<body>
    <h1>Гостевая книга</h1>

    <div class="form-container">
        <form method="POST">
            <label for="name">Имя: </label><br>
            <input type="text" id="name" name="name" required><br><br>

            <label for="message">Сообщение: </label><br>
            <textarea id="message" name="message" rows="4" required></textarea><br><br>

            <button type="submit">Отправить</button>
        </form>

        <?php if (!empty($error)): ?>
            <p class="error"><?= $error ?></p>
        <?php endif; ?>

        <?php if (!empty($success)): ?>
            <p class="success"><?= $success ?></p>
        <?php endif; ?>
    </div>

    <div class="messages">
        <h2>Сообщения:</h2>
        <?php if (!empty($entries)): ?>
            <?php foreach ($entries as $entry): ?>
                <div class="message">
                    <h4><?= htmlspecialchars($entry['name']) ?> <small>(<?= htmlspecialchars($entry['date']) ?>)</small></h4>
                    <p><?= nl2br(htmlspecialchars($entry['message'])) ?></p>
                </div>
            <?php endforeach; ?>
        <?php else: ?>
            <p>Сообщений пока нет.</p>
        <?php endif; ?>
    </div>
</body>
</html>
