const screenshot = require('screenshot-desktop');
const robot = require('robotjs');
const cv = require('opencv4nodejs');
const tesseract = require('tesseract.js');

// Funkcja do przechwycenia całego ekranu
async function captureScreen() {
  const imgBuffer = await screenshot({ format: 'png' });
  return cv.imdecode(imgBuffer); // Zamiana bufora na obraz OpenCV
}

// Funkcja do wyboru obszaru ekranu
function getSelectedRegion() {
  console.log('Wybierz górny-lewy i dolny-prawy punkt (na ekranie).');
  
  const start = robot.getMousePos(); // Pobiera współrzędne kursora (start)
  console.log('Początek: ', start);
  console.log('Przesuń myszkę na dolny-prawy punkt i naciśnij Enter.');

  // Czekanie na zakończenie wyboru
  return new Promise((resolve) => {
    process.stdin.once('data', () => {
      const end = robot.getMousePos(); // Pobiera współrzędne końcowe
      console.log('Koniec: ', end);

      resolve({
        x: start.x,
        y: start.y,
        width: end.x - start.x,
        height: end.y - start.y,
      });
    });
  });
}

// Funkcja przetwarzająca Sudoku z wybranego obszaru
async function processSudoku(region) {
  const screen = await captureScreen();
  const selectedArea = screen.getRegion(new cv.Rect(region.x, region.y, region.width, region.height));

  // Konwersja do odcieni szarości
  const gray = selectedArea.bgrToGray();

  // Wykrywanie siatki Sudoku
  const edges = gray.canny(50, 150);
  const contours = edges.findContours(cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE);
  const largestContour = contours.sort((c1, c2) => c2.area - c1.area)[0];

  if (largestContour) {
    const boundingRect = largestContour.boundingRect();
    const sudokuGrid = selectedArea.getRegion(boundingRect).resize(450, 450);

    // Rozpoznawanie cyfr w komórkach
    const cellSize = 50;
    const recognizedNumbers = [];
    for (let row = 0; row < 9; row++) {
      for (let col = 0; col < 9; col++) {
        const cell = sudokuGrid.getRegion(new cv.Rect(col * cellSize, row * cellSize, cellSize, cellSize));
        const cellGray = cell.bgrToGray();

        const { data: { text } } = await tesseract.recognize(cv.imencode('.jpg', cellGray).toString('base64'), {
          lang: 'eng',
        });

        const recognizedDigit = parseInt(text.trim()) || 0;
        recognizedNumbers.push(recognizedDigit);
      }
    }

    // Wyświetlanie macierzy Sudoku
    const sudokuMatrix = [];
    for (let i = 0; i < 9; i++) {
      sudokuMatrix.push(recognizedNumbers.slice(i * 9, (i + 1) * 9));
    }

    console.log('Rozpoznane Sudoku:');
    console.table(sudokuMatrix);
    return sudokuMatrix;
  }

  console.error('Nie znaleziono siatki Sudoku.');
  return null;
}

// Główna logika
(async () => {
  const region = await getSelectedRegion(); // Użytkownik wybiera obszar
  console.log('Wybrany obszar: ', region);

  await processSudoku(region); // Przetwarzanie Sudoku
})();
