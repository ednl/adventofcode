inp = 'To continue, please consult the code grid in the manual.  Enter the code at row 2947, column 3029.'

def getindex(row: int, col: int) -> int:   # row/col are one-based
    tri = row + col - 2                    # triangle base is zero-based
    return col - 1 + tri * (tri + 1) // 2  # number of spots in de triangle

val = 20151125  # starting value at grid pos (1,1) = index 0
mul = 252533    # multiplier (base of the modular exponentiation)
mod = 33554393  # modulus of the mod. exp.
pos = getindex(2947, 3029)  # destination pos from the input (exponent of the mod. exp.)

# Right answer: 19980801
print(val * pow(mul, pos, mod) % mod)
