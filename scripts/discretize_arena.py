import sys
import math

def split_arena(arena_size):
    assert (arena_size > 0), "Arena size needs to be positive"

    BLOCK_SIZES = [1024, 512, 256, 128, 64, 32]
    blocks = {}
    remaining = arena_size

    if arena_size < min(BLOCK_SIZES):
        return blocks, remaining

    # TODO: Alternate solution: This can be made into an optimization problem.
    #       - Constraint is that the sum of the allocations to the block sizes
    #         must be ≤ arena_size.
    #       - Minimize the mean and standard deviation byte distribution /w
    #         respect to a desired distribution (e.g, uniform, normal, etc.).

    # Solution 2: The Alternative Walk Solution
    # Based on the arena size, some block sizes are just not possible or would
    # take up too much of the arena. For example, I would rather split an arena
    # size of 512 bytes among the 128, 256, and 64 sizes, rather than entirely
    # into 512.
    # Then, I will try to distribute the arena from the middle of the remaining
    # list outward in both directions, doing a sort of alternating walk. I will
    # select an appropriate "pivot" size that represents the middle of the
    # distribution.
    workable_blk_szs = [sz for sz in BLOCK_SIZES if sz <= (arena_size // 2)]
    split_sz = arena_size / len(workable_blk_szs)
    pivot_sz = min( workable_blk_szs, key=lambda x : abs(x - split_sz) )
    # Now, the alternative walk
    mid = (len(workable_blk_szs) // 2) - 1 # take one off for 0-based indexing
    # Create a list of indices that represent the sequence of indices to visit
    # for the walk. Treat this list like a tree where each index in the list
    # represents the pair of indices that live at level i of the tree.
    indices = [ (mid,None) ]
    for dist in range(1, math.ceil(len(workable_blk_szs)/2)):
        # Start on the left of mid, then the right
        this_level = []
        if (mid - dist) >= 0:
            this_level.append(mid - dist)
        else:
            this_level.append(None)
        if (mid + dist) < len(workable_blk_szs):
            this_level.append(mid + dist)
        else:
            this_level.append(None)
        indices.append(tuple(this_level))
        

    # Solution 3: Give to the Big Boys First
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
