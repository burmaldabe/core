# Error handling

Any function that performs an __action__ (i.e., a procedure, but there is
no such concept in C) rather than calculating/converting a value, must
return only __two__ possible values: **0 in case of success and everything
else (typically 1) in case of failure**. Information about the cause of the
failure should be one of the output parameters.

If such a function returns exit status, but doesn't have an output
parameter with the cause of the failure, then its cause in the **errno
variable**. There are __no exceptions__ to this rule.
