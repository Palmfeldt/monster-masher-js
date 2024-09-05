import React from 'react';
import '/sercGameBoard.css';
const GameBoard: React.FC = () => {

  const height = 10;
  const width = 15;
  const grid: boolean[][] = [];

  // Generate the grid with random values
  for (let i = 0; i < height; i++) {
    const row: boolean[] = [];
    for (let j = 0; j < width; j++) {
      const isFilled = Math.random() < 0.5; // 50% chance of being filled
      row.push(isFilled);
    }
    grid.push(row);
  }

  return (
    <div>
      {grid.map((row, rowIndex) => (
        <div className='test' key={rowIndex}>
          {row.map((isFilled, colIndex) => (
            <div
              key={colIndex}
              style={{
                width: '20px',
                height: '20px',
                backgroundColor: isFilled ? 'black' : 'white',
                display: 'inline-block',
              }}
            />
          ))}
        </div>
      ))}
    </div>
  );
};

export default GameBoard;