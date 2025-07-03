import sys
import math

def split_arena(arena_size):
    assert (arena_size > 0), "Arena size needs to be positive"

    BLOCK_SIZES = [1024, 512, 256, 128, 64, 32]
    blocks = {}
    remaining = arena_size

    if arena_size < min(BLOCK_SIZES):
        return blocks, remaining

    # Based on the arena size, some block sizes are just not possible. Ruling
    # those out, I will try to distribute the arena across the middle of the
    # remaining list outward in both directions.
    # I will select an appropriate "pivot" size that represents the middle of
    # the distribution. From there, I will distribute in an alternating fashion
    # between the left of the pivot and the right.
    # To facilitate that, I'd like an iterator that knows to alternate indices
    # in this manner. Best way I can figure right now is to just create the list
    # of indices in the sequence I want.
    workable_blk_szs = [sz for sz in BLOCK_SIZES if sz <= (arena_size // 2)]
    split_sz = arena_size / len(workable_blk_szs)
    pivot_idx, pivot_sz = min( enumerate(BLOCK_SIZES),
                               key=lambda x : abs(x[1] - split_sz) )
    mid = len(BLOCK_SIZES) // 2
    indices = [mid]
    for dist in range(1, mid+1):
        if (mid - dist) 

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
