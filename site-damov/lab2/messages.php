<?php
if (file_exists('messages.txt')) {
    $messages = file_get_contents('messages.txt');
    echo nl2br($messages);
} else {
    echo 'Нет сообщений.';
}
?>
