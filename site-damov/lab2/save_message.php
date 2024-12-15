<?php
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $name = htmlspecialchars($_POST['name']);
    $message = htmlspecialchars($_POST['message']);
    $date = date('Y-m-d H:i:s');

    $message_data = "{$date} | {$name}: {$message}\n";

    file_put_contents('messages.txt', $message_data, FILE_APPEND);

    header('Location: guestbook.php');
    exit;
}
?>
