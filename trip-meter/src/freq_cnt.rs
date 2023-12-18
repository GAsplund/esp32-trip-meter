use core::cell::RefCell;
use critical_section::Mutex;
use hal::{
    peripherals::TIMG0,
    prelude::*,
    timer::{Timer, Timer0}, gpio::{Event, self},
};

pub struct FreqCnt {
    latest_rise: u64,
    latest_fall: u64,
    latest_period: u32,
    latest_duty: u32,
}

static TIMER: Mutex<RefCell<Option<Timer<Timer0<TIMG0>>>>> = Mutex::new(RefCell::new(None));

impl FreqCnt {
    pub fn new() -> Self {
        FreqCnt {
            latest_rise: 0,
            latest_fall: 0,
            latest_period: 0,
            latest_duty: 0,
        }
    }

    pub fn install_freq_only(&self, mut gpio: impl gpio::Pin) {
        gpio.listen(Event::RisingEdge);
    }

    pub fn install_with_duty(&self, mut gpio: impl gpio::Pin) {
        gpio.listen(Event::RisingEdge);
        gpio.listen(Event::FallingEdge);
    }

    pub fn rise(&mut self) {
        critical_section::with(|cs| {
            let mut timer = TIMER.borrow_ref_mut(cs);
            let timer = timer.as_mut().unwrap();
            let time = timer.now();
            self.latest_period = (time - self.latest_rise) as u32;
            self.latest_rise = time;
        });
    }

    pub fn fall(&mut self) {
        critical_section::with(|cs| {
            let mut timer = TIMER.borrow_ref_mut(cs);
            let timer = timer.as_mut().unwrap();
            self.latest_fall = timer.now();
        });
        self.latest_duty = (self.latest_fall - self.latest_rise) as u32;
    }

    pub fn get_freq(&self) -> u32 {
        self.latest_period * ( 1 / 1000000 )
    }

    pub fn get_duty(&self) -> f32 {
        self.latest_duty as f32 / self.latest_period as f32
    }
}
