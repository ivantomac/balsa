;;;
;;;	The Balsa Asynchronous Hardware Synthesis System
;;;	Copyright (C) 1995-2008 School of Computer Science
;;;	The University of Manchester, Oxford Road, Manchester, UK, M13 9PL
;;;	
;;;	This program is free software; you can redistribute it and/or modify
;;;	it under the terms of the GNU General Public License as published by
;;;	the Free Software Foundation; either version 2 of the License, or
;;;	(at your option) any later version.
;;;	
;;;	This program is distributed in the hope that it will be useful,
;;;	but WITHOUT ANY WARRANTY; without even the implied warranty of
;;;	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;;	GNU General Public License for more details.
;;;	
;;;	You should have received a copy of the GNU General Public License
;;;	along with this program; if not, write to the Free Software
;;;	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
;;;
;;;	`misc-date.scm'
;;;	Date/time manipulation procedures
;;;

;;; time/date constants
(define secs-per-day 86400)
(define first-day-of-month '#(1 32 60 91 121 152 182 213 244 274 305 335 366))
(define first-day-of-month-in-leap-year '#(1 32 61 92 122 153 184 214 245 275 306 336 367))
(define month-names '#("Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec" "Gel"))
(define day-names '#("Sun" "Mon" "Tue" "Wed" "Thu" "Fri" "Sat"))

;;; day-of-19700101: 1/1/1970 was a Thursday
(define day-of-19700101 3)

;;; leap-years-to: leap years from (non-existant) year 0 to given year. Year 0 _was_ a leap year.
(define leap-years-to (lambda (year)
	(let
		((fourth-years (quotient year 4))
		 (hundredth-years (quotient year 100))
		 (four-hundredth-years (quotient year 400))
		)
		(+ (- fourth-years hundredth-years) four-hundredth-years)
	)
))

;;; leap-years-to-1970: leap years between 0 and 1970 (inclusive)
(define leap-years-to-1970 (leap-years-to 1970))

;;; leap-years-since-1970: eg. (self 1975) => 1
(define leap-years-since-1970 (lambda (year) (- (leap-years-to (+ 1970 year)) leap-years-to-1970)))

;;; is-leap-year?: is the given year (full year no eg. 1988) a leap year?
(define is-leap-year? (lambda (year)
	(or
		(and (zero? (remainder year 4)) (not (zero? (remainder year 100))))
		(zero? (remainder year 400))
	)
))

;;; find-month: given a day number in the year (1 based) and an indication of whether
;;;		or not this year is a leap year: return a pair (month-no . month-day)
;;;		`month-no' is the 0 based month number.  `month-day' is a 1 based month day number
(define find-month (lambda (day-no is-leap-year)
	(let
		((month-start-days (if is-leap-year
			first-day-of-month-in-leap-year
			first-day-of-month
		)))
		(letrec
			((find-month-body (lambda (try-month)
				(if (> (vector-ref month-start-days try-month) day-no)
					(cons (- try-month 1) (- day-no (vector-ref month-start-days (- try-month 1)) -1))
					(find-month-body (+ 1 try-month))
				)
			)))
			(find-month-body 0)
		)
	)
))

;;; month-name: name for month no. `month'
(define month-name (lambda (month) (vector-ref month-names month)))
;;; day-of-week: name for day no. `day'
(define day-of-week (lambda (day) (vector-ref day-names day)))

;;; asctime: return a current time string from a given expanded-time
(define asctime (lambda (expanded-date)
	(let*
		((two-digit-pad (lambda (val) (overwrite-end-of-string "00" (number->string val))))
		 (year (+ 1900 (expanded-date:year expanded-date)))
		 (time-zone (if (= 11 (vector-length expanded-date)) ; localtime result rather than expanded-date
			(vector-ref expanded-date 10) "GMT"))
		)
		(string-append
			(day-of-week (expanded-date:wday expanded-date)) " "
			(month-name (expanded-date:mon expanded-date)) " "
			(overwrite-end-of-string "  " (number->string (expanded-date:mday expanded-date))) " "
			(two-digit-pad (expanded-date:hour expanded-date)) ":"
			(two-digit-pad (expanded-date:min expanded-date)) ":"
			(two-digit-pad (expanded-date:sec expanded-date)) " "
			time-zone " "
			(number->string year)
		)
	)
))

;;; ctime: (ctime) returns the current time. (ctime time-t) returns the time string for
;;;		the given time_t.
(define ctime (lambda args
	(let*
		((time-t (if (null? args) (current-time) (car args)))
		 (expanded-date (localtime time-t))
		)
		(asctime expanded-date)
	)
))

;;; localtime-fallback: expand a time_t out to a list of:
;;;		(year-number is-leap-year month-number month-short-name month-day-number hour minutes secs)
;;;		NB. month-number is 0 based, as are hour, minutes and secs.  month-day-number is 1 based.
;;;		eg. (expand-data 0) => (1970 #f 0 "Jan" 1 0 0 0),
;;;		(localtime 894077849 => (1998 #f 4 "May" 2 2 57 29)
(define localtime-fallback (lambda (time-t)
	(let*
		((days (quotient time-t secs-per-day))
		 (rough-years (quotient days 365))
		 (leap-between-1970-and-rough (leap-years-since-1970 rough-years))
		 ; I think this calc. is OK for about 1400 years
		 (un-leaping-days (- days leap-between-1970-and-rough))
		 (year-no (quotient un-leaping-days 365))
		 (day-no (+ 1 (remainder un-leaping-days 365)))
		 (year (+ 1970 year-no))
		 (leap (is-leap-year? year))
		 (month/month-day-no (find-month day-no leap))
		 (secs-since-midnight (remainder time-t secs-per-day))
		 (hour (quotient secs-since-midnight 3600))
		 (secs-since-hour (remainder secs-since-midnight 3600))
		 (minutes (quotient secs-since-hour 60))
		 (secs (remainder secs-since-hour 60))
		)
		(vector
			secs minutes hour
			(cdr month/month-day-no)
			(car month/month-day-no)
			(- year 1900)
			(remainder (+ day-of-19700101 days) 7)
			day-no
		)
	)
))

;;; FIXME, write a gmtime-fallback

;;; expanded-date:...: date field accessors
(define expanded-date:sec (vector-nth 0))
(define expanded-date:min (vector-nth 1))
(define expanded-date:hour (vector-nth 2))
(define expanded-date:mday (vector-nth 3))
(define expanded-date:mon (vector-nth 4))
(define expanded-date:year (vector-nth 5))
(define expanded-date:wday (vector-nth 6))
(define expanded-date:yday (vector-nth 7))

