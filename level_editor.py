import pygame, sys, time, math

SCR_WIDTH = 2000
SCR_HEIGHT = 1000

TILE_SIZE = 8
CHUNK_SIZE = 9
LEVEL_WIDTH = 20
LEVEL_HEIGHT = 20

MAP = "0.json"

class Editor:
    def __init__(self):
        self.dimensions = pygame.Vector2(SCR_WIDTH, SCR_HEIGHT)
        # render surfaces
        self.screen = pygame.Surface(self.dimensions // 2)
        self.display = pygame.display.set_mode(self.dimensions)
        # clock
        self.clock = pygame.time.Clock()
        # scroll
        self.scroll = pygame.Vector2(0, 0)
        self.controls = {"right": False, "left": False, "up": False, "down": False}

        # time step
        self.dt = 1
        self.last_time = time.time() - 1/60

        # flags
        self.running = True

        # level data
        self.tiles = {}

        # assets
        self.assets = {
            "tiles/dirt": self.load_tileset(pygame.image.load("data/images/tiles/dust.png").convert())
        }

    def close(self):
        self.running = False
        pygame.quit()
        sys.exit()
    
    def draw_tile_grid(self, scroll, size, color):
        tile_size = [TILE_SIZE * size[0], TILE_SIZE * size[1]]
        length = math.ceil(self.screen.get_width() / tile_size[0]) + 2
        height = math.ceil(self.screen.get_height() / tile_size[1]) + 2
        for x in range(length):
            pygame.draw.line(self.screen, color, ((x - 1) * tile_size[0] - (scroll[0] % tile_size[0]), 0), ((x - 1) * tile_size[0] - (scroll[0] % tile_size[0]), self.screen.get_height()))
        for y in range(height):
            pygame.draw.line(self.screen, color, (0, (y - 1) * tile_size[1] - (scroll[1] % tile_size[1])), (self.screen.get_width(), (y - 1) * tile_size[1] - (scroll[1] % tile_size[1])))
    
    def draw_grid(self):
        self.draw_tile_grid(self.scroll, [1, 1], (50, 50, 50))
        self.draw_tile_grid(self.scroll, [CHUNK_SIZE, CHUNK_SIZE], (100, 100, 255))
        pygame.draw.line(self.screen, (255, 255, 255), (-self.scroll.x, -self.scroll.y), (LEVEL_WIDTH * CHUNK_SIZE * TILE_SIZE - self.scroll.x, -self.scroll.y), 1)
        pygame.draw.line(self.screen, (255, 255, 255), (-self.scroll.x, -self.scroll.y), (-self.scroll.x, LEVEL_HEIGHT * CHUNK_SIZE * TILE_SIZE - self.scroll.y), 1)
        pygame.draw.line(self.screen, (255, 255, 255), (LEVEL_WIDTH * CHUNK_SIZE * TILE_SIZE - self.scroll.x, -self.scroll.y), (LEVEL_WIDTH * CHUNK_SIZE * TILE_SIZE - self.scroll.x, LEVEL_HEIGHT * CHUNK_SIZE * TILE_SIZE - self.scroll.y), 1)
        pygame.draw.line(self.screen, (255, 255, 255), (-self.scroll.x, LEVEL_HEIGHT * CHUNK_SIZE * TILE_SIZE - self.scroll.y), (LEVEL_WIDTH * CHUNK_SIZE * TILE_SIZE - self.scroll.x, LEVEL_HEIGHT * CHUNK_SIZE * TILE_SIZE - self.scroll.y), 1)
    
    def load_tileset(self, sheet):
        tiles = []
        for y in range(4):
            for x in range(4):
                tile_surf = pygame.Surface((TILE_SIZE, TILE_SIZE))
                tile_surf.blit(sheet, (-x * TILE_SIZE, -y * TILE_SIZE))
                tile_surf.set_colorkey((0, 0, 0))
                tiles.append(tile_surf)
        return tiles

    def load_sheet(self, sheet, tile_size):
        tiles = []
        for x in range(math.floor(sheet.get_width() / tile_size[0])):
            tile_surf = pygame.Surface(tile_size)
            tile_surf.blit(sheet, (-x * tile_size[0], 0))
            tile_surf.set_colorkey((0, 0, 0))
            tiles.append(tile_surf)
        return tiles
    
    def update(self):
        self.scroll.x += (int(self.controls['right']) - int(self.controls['left'])) * 5 * self.dt
        self.scroll.y += (int(self.controls['down']) - int(self.controls['up'])) * 5 * self.dt

        self.screen.fill((0, 0, 0))
        self.draw_grid()

    def run(self):
        while self.running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.close()
                elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_ESCAPE:
                        self.close()
                    if event.key == pygame.K_RIGHT or event.key == pygame.K_d:
                        self.controls["right"] = True
                    if event.key == pygame.K_LEFT or event.key == pygame.K_a:
                        self.controls["left"] = True
                    if event.key == pygame.K_UP or event.key == pygame.K_w:
                        self.controls["up"] = True
                    if event.key == pygame.K_DOWN or event.key == pygame.K_s:
                        self.controls["down"] = True
                elif event.type == pygame.KEYUP:
                    if event.key == pygame.K_RIGHT or event.key == pygame.K_d:
                        self.controls["right"] = False
                    if event.key == pygame.K_LEFT or event.key == pygame.K_a:
                        self.controls["left"] = False
                    if event.key == pygame.K_UP or event.key == pygame.K_w:
                        self.controls["up"] = False
                    if event.key == pygame.K_DOWN or event.key == pygame.K_s:
                        self.controls["down"] = False

            self.update()

            pygame.transform.scale2x(self.screen, self.display)
            pygame.display.set_caption(f"FPS: {self.clock.get_fps() :.1f}")
            pygame.display.flip()
            self.clock.tick()

            # update deltatime
            self.dt = time.time() - self.last_time
            self.dt *= 60
            self.last_time = time.time()

if __name__ == "__main__":
    Editor().run()