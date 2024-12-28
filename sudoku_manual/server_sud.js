// Importujemy wymagane moduły
const express = require('express');
const path = require('path');

// Tworzymy aplikację Express
const app = express();
const PORT = 3000;

// Ustawiamy ścieżkę do folderu statycznego
app.use(express.static(path.join(__dirname, 'public')));

// Parsowanie danych z formularzy
app.use(express.urlencoded({ extended: true }));

// Obsługa strony głównej
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// Uruchamiamy serwer
app.listen(PORT, () => {
    console.log(`Serwer działa na http://localhost:${PORT}`);
});
