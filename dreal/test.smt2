(set-option :print-success true)
(set-logic QF_NRA)
(declare-fun x () Real)
(assert (> x 0.0))
(assert (< x 5.0))
(check-sat)
(get-model)
(assert (> x 2.0))
(push)
(assert (> x 10.0))
(check-sat)
(pop)
(check-sat)
(get-model)
