#lang racket
(define cooly '(0 1 2 4 3 4 7 9))
(define again '(9 9 9 4 2 6 8 7))
(define listA '(3 7 12 5 9 12 4 8 6 12 10 2 14 12 1 11 13 12 15 12 16))
(define listB  '(25 18 30 22 25 17 19 25 21 20 18 24 23 25 26 27 18 29 28 25))

; returns the i element of the list
(define (geti lis i)
  (if (= i 0)
      (car lis)
      (geti (cdr lis) (- i 1))))

;geti using cond
(define (geticond lis i)
  (cond
    ((>= i (length lis)) (print "Error, indexoutofboundsexception") #f)
    ((= i 0) (car lis))
    (else
     (geti (cdr lis) (- i 1)))))

; returns index of first occurence of val in the list
; -1 if not found
(define (findv lis val)
  (findvHelper lis val 0))

; recursive helper for findv
; lis - list we are searching
; val - value we are searching for
; i - where we are starting/how far we have made it
; base case 1 - i == (length lis); didnt find, return -1
; base case 2 - (car lis) == val; found, return i
; recursive step - get findvHelper of cdr lis, val, (+ i 1)
(define (findvHelper lis val i)
  (cond
    ((= i (length lis)) -1)
    ((= (geti lis i) val) i) ;use geti because we arent using cdr lis, so lis doesnt get shorter automatically
    (else (findvHelper lis val (+ i 1)))))
  
      
; returns shortened list with numbers that were in even positions only
(define (evenPositionsOnly lis)
  (evenPosOnlyHelper lis 0))

; base case - return empty list
; if i is even, return a constructed list with the first element of the list appended to what is returned by the recursive call
; else, do a call again
(define (evenPosOnlyHelper lis i)
  (cond
    ((empty? lis) '())
    ((even? i) (cons (car lis) (evenPosOnlyHelper (cdr lis) (+ i 1))))
    (else (evenPosOnlyHelper (cdr lis) (+ i 1)))))

;remind of how filter works:
; applies a function that takes in a single parameter to each element in the list
; if the function returns true, that element is kept; if false, element is discarded
(define (evenVals lis)
  (filter even? lis))

;doesnt work with duplicates
;there is a way
;you have to search for the element and then see if the index was odd or even
(define (evenPos lis)
  (filter
   (lambda (element)
     (even? (findv lis element))) lis))


;sort time
; base case - list is empty
; two recursive steps
; 1. if first item is smallest item, then cons it to sorted tail of list
; 2. if not the smallest, then return sorted entire list but with first item moved to end
(define (sortL lis)
  (cond
    ((empty? lis) lis)
    ((= (car lis) (apply min lis)) (cons (car lis) (sortL (cdr lis))))
    (else
     (sortL (append (cdr lis) (list (car lis)))))))

;mode calc
;base case - empty list
;recursive step: if car lis = element, add one to whatever the call returns
;else call again
(define (countElement lis element)
  (cond
    ((empty? lis) 0)
    ((= (car lis) element) (+ 1 (countElement (cdr lis) element)))
    (else (countElement (cdr lis) element))))

;mapping function using count element
(define (mapCountElement lis)
  (map (lambda (element) (countElement lis element)) lis))

;get max function
(define (maxl lis)
  (if (= (length lis) 1)
      (car lis)
      (if (> (car lis) (maxl (cdr lis))) ;if the first element of the list is less than a recursive call to the rest of the list, return first element
          (car lis) ;but the comparison cant be made until all recursive calls finish
          (maxl (cdr lis)))))

;Finally our mode
;Format: number, amount of occurences
(define (mode lis)
  (define x (maxl (mapCountElement lis)))
  (define y (findv (mapCountElement lis) x))
  (define z (geti lis y))
  (cons z x))

(define (minl lis)
  (if (= (length lis) 1)
      (car lis)
      (if (< (car lis) (minl (cdr lis))) ;if the first element of the list is less than a recursive call to the rest of the list, return first element
          (car lis) ;but the comparison cant be made until all recursive calls finish
          (minl (cdr lis))))) ;a second recursive call, inefficient

; recursive function to add all numbers in a list
(define (suml lis)
  (if (= (length lis) 1) ;check if lengh of list is 1 (in lisp)
      (car lis) ;provide first element of list with car function
      (+ (car lis) (suml (cdr lis)))))

;avg 
(define (avgl lis)
  (if (empty? lis) ;error checking
      (print "error")
      (/ (suml lis) (length lis))))

;median
(define (medl lis)
  (define slis (sortL lis))
  (cond
    ((= (modulo (length lis) 2) 1) (geti slis (/ (- (length slis) 1) 2)))
    ((= (modulo (length lis) 2) 0)
     (define w (/ (length lis) 2))
     (define x (- (/ (length lis) 2) 1))
     (define y (geti slis w))
     (define z (geti slis x))
     (/ (+ y z) 2))))

;calls
(displayln "ListA Results: ")
(avgl listA)
(medl listA)
(mode listA)
(minl listA)
(maxl listA)
(displayln "-------")
(displayln "ListB Results: ")
(avgl listB)
(medl listB)
(mode listB)
(minl listB)
(maxl listB)
