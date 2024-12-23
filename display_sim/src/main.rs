use std::io::{self, Write};
use crossterm::{
    ExecutableCommand, QueueableCommand,
    terminal, cursor, style::{self, Stylize, Color} 
};
use chrono::prelude::*;

const N_ROWS : usize = 2;
const N_COLUMNS : usize = 6;

struct Display {
  contents : [[Color; N_COLUMNS]; N_ROWS]
}

impl Display {
  fn new() -> Display {
    Display {contents: [[Color::Grey; N_COLUMNS]; N_ROWS] }  
  }
}

fn main() -> io::Result<()> {
  let mut stdout = io::stdout();

  stdout.execute(terminal::Clear(terminal::ClearType::All))?;
  let mut display = Display::new();
  set_time_bin(&mut display);
  for y in 0..N_ROWS {
    for x in 0..N_COLUMNS {
        stdout
          .queue(cursor::MoveTo(x as u16,y as u16))?
          .queue(style::PrintStyledContent( "⬤".with(display.contents[y][x])))?;
    }
  }

  stdout.queue(cursor::MoveTo(0, N_ROWS as u16))?;
  stdout.flush()?;
  Ok(())
} 

fn set_time_bin(display: &mut Display) {
  let local: DateTime<Local> = Local::now(); 
  set_int_in_row(display, local.hour(), Color::Red, 0);
  set_int_in_row(display, local.minute(), Color::Green, 1);
}

fn set_int_in_row(display: &mut Display, value: u32, color: Color, row: usize) {
  
  for bit in 0..N_COLUMNS {
    if value & (1 << bit) != 0 {
      display.contents[row][N_COLUMNS - 1 - bit] = color;  
    }
  }
}

