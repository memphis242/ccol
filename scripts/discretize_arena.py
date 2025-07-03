import sys
import math

def split_arena(arena_size):
    assert (arena_size > 0), "Arena size needs to be positive"

    BLOCK_SIZES = [1024, 512, 256, 128, 64, 32]
    blocks = {}
    remaining = arena_size

    if arena_size < min(BLOCK_SIZES):
        return blocks, remaining

    #############################################
    # TODO: Solution 1: The Optimization Solution
    #############################################
    #       - Constraint is that the sum of the allocations to the block sizes
    #         must be ≤ arena_size.
    #       - Minimize the mean and standard deviation byte distribution /w
    #         respect to a desired distribution (e.g, uniform, normal, etc.).

    ############################################
    # Solution 2: The Alternating Walk Solution
    ############################################
    # Based on the arena size, some block sizes are just not possible or would
    # take up too much of the arena. For example, I would rather split an arena
    # size of 512 bytes among the 128, 256, and 64 sizes, rather than entirely
    # into 512. For a given arena size, I will select a paritioning that best
    # evenly distributes the arena out, favoring the low-to-mid sizes.
    # There are many ways to parition the arena among the available sizes. Let's
    # find which way gives us the widest dsitribution.
    workables = []
    for partition in range(len(BLOCK_SIZES), 0, -1):
        dist_sz = max(
            [sz for sz in BLOCK_SIZES if sz <= (arena_size // partition)],
            default=0 )
        if dist_sz == 0:
            break
        tmp = [sz for sz in BLOCK_SIZES if sz <= dist_sz ]
        cumulation = 0
        lst = []
        for sz in tmp:
            cumulation += dist_sz
            if cumulation <= arena_size:
                lst.append(sz)
            else:
                break
        workables.append( (lst, dist_sz) )
    # Get the longest list - this represents the "widest" distribution
    workable, distribution_sz = max( workables, key=lambda x: len(x[0]) )

    # Now that a partionining and distribution size has been decided on, I will
    # begin distributing from the mid of the list outwards, going one to the
    # left of the mid, then one to the right, then two the left, and so on,
    # until the arena is exhausted and a gap less than the smallest size
    # remains. I am calling this "walk" through the sizes the "alternating walk".
    mid = (len(workable) // 2) - 1 # take one off to favor lower sizes first
    # Create a list of indices that represent the sequence of indices to visit
    # for the walk. Treat this list like a tree where each index in the list
    # represents the pair of indices that live at level i of the tree.
    idx_tree = [ (mid,None) ]
    for dist in range(1, math.ceil(len(workable)/2)):
        # Start on the left of mid, then the right
        this_lvl = []
        if (mid - dist) >= 0:
            this_lvl.append(mid - dist)
        else:
            this_lvl.append(None)
        if (mid + dist) < len(workable):
            this_lvl.append(mid + dist)
        else:
            this_lvl.append(None)
        idx_tree.append(tuple(this_lvl))

    # Now, we breadth-first walk!
    while remaining >= min(workable):
        for lvl in idx_tree:
            blocks[ workable[lvl[0]] ] += 1

    #################################################
    # Solution 3: Give to the Big Boys First Solution
    #################################################
    for size in BLOCK_SIZES:
        count, remaining = divmod(remaining, size)
        blocks[size] = count

    assert sum(size * blocks[size] for size in BLOCK_SIZES) + remaining == arena_size, \
        "Block sum and gap do not match arena size"
    return blocks, remaining

def print_split(arena_size):
    MAGENTA = "\033[1;35m"
    CYAN = "\033[1;36m"
    RED = "\033[1;31m"
    RESET = "\033[0m"
    GREEN = "\033[1;32m"

    blocks, gap = split_arena(arena_size)
    print(f"Arena size: {GREEN}{arena_size}{RESET} bytes")
    for size in sorted(blocks, reverse=True):
        print(f"{MAGENTA}{size:4}{RESET} byte blocks: {CYAN}{blocks[size]} ({blocks[size] * size} bytes){RESET}")
    print(f"Remaining gap: {RED}{gap} bytes{RESET}")

    # Visual representation
    print("\nVisual layout:")
    max_block_width = 32  # Increase for better proportionality

    def get_width(size):
        return int(max_block_width * (size / 1024))

    visual_chunks = []
    current_line = ""
    current_len = 0

    def append_block(block_str):
        nonlocal current_line, current_len, visual_chunks
        block_len = len(block_str.encode('utf-8'))  # Use byte length for accuracy with wide chars
        if current_len + block_len > 256:
            visual_chunks.append(current_line)
            current_line = block_str
            current_len = block_len
        else:
            current_line += block_str
            current_len += block_len

    for size in sorted(blocks, reverse=True):
        count = blocks[size]
        if count == 0:
            continue
        width = get_width(size)
        label = f"{size}"
        block_str = f"{MAGENTA}[{label.center(width-2)}]{RESET}"
        for _ in range(count):
            append_block(block_str)

    if gap > 0:
        gap_width = get_width(gap)
        gap_label = "GAP" if gap_width >= 7 else ""
        gap_str = f"{RED}[{gap_label.center(gap_width-2)}]{RESET}"
        append_block(gap_str)

    if current_line:
        visual_chunks.append(current_line)

    for line in visual_chunks:
        print(line)
    print()  # Extra newline for clarity

if __name__ == "__main__":
    if len(sys.argv) != 2 or not sys.argv[1].isdigit():
        print("Usage: python colorize_arena.py <arena_size_in_bytes>")
        sys.exit(1)
    arena = int(sys.argv[1])
    print_split(arena)
