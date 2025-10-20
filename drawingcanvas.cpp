#include "drawingcanvas.h"

static const vector<pair<string, std::array<std::array<bool, 3>, 3>>> segment_patterns = {
    {"Horizontal Line (Center)", {{
        {0, 0, 0},
        {1, 1, 1},
        {0, 0, 0}
    }}},
    {"Vertical Line (Center)",{{
        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0}
    }}},
    {"Diagonal (Top-Left to Bottom-Right)",{{
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    }}},
    {"Diagonal (Top-Right to Bottom-Left)",{{
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 0}
    }}},
    {"L-Shape (Top-Left)", {{
        {1, 1, 0},
        {1, 0, 0},
        {0, 0, 0}
    }}},
    {"Single Point (Center)", {{
        {0, 0, 0},
        {0, 1, 0},
        {0, 0, 0}
    }}}
};

bool isMatrixEmpty(const CustomMatrix& matrix) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (matrix.mat[i][j]) {
                return false;
            }
        }
    }
    return true;
}

void printMatrix(const CustomMatrix& matrix) {
    cout << "------" << endl;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cout << (matrix.mat[i][j] ? '1' : '0') << " ";
        }
        cout << endl;
    }
    cout << "------" << endl;
}

string DrawingCanvas::isSegmentPattern(const CustomMatrix& matrix){
    for (const auto& pattern : segment_patterns){
        bool match = true;
        for (int i = 0; i < 3; ++i){
            for(int j = 0; j < 3; ++j){
                if (matrix.mat[i][j] != pattern.second[i][j]){
                    match = false;
                    break;
                }
            }
            if (!match) break;
        }
        if (match){
            return pattern.first;
        }
        return "";
    }
}

DrawingCanvas::DrawingCanvas(QWidget *parent) : QWidget(parent) {
    // Set a minimum size for the canvas
    setMinimumSize(this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
    // Set a solid background color
    setStyleSheet("background-color: white; border: 1px solid gray;");
}

void DrawingCanvas::clearPoints(){
    m_points.clear();
    m_detectedSegments.clear();
    // Trigger a repaint to clear the canvas
    update();
}

void DrawingCanvas::paintLines(){
    /* Todo
     * Implement lines drawing per even pair
    */

    isPaintLinesClicked = true;
    update();
}

void DrawingCanvas::segmentDetection(){
    QPixmap pixmap = this->grab(); //
    QImage image = pixmap.toImage();

    m_detectedSegments.clear();

    cout << "\n--- Segment Detection Report ---" << endl;
    cout << "Image size: " << image.width()<< "x" << image.height() << endl;
    cout << "\n*** Non-Empty Window Dump (Goal 1 & 2) ***" << endl;

    //To not crash we set initial size of the matrix
    vector<CustomMatrix> windows;
    windows.reserve((image.width()-2)*(image.height()-2));

    // Get the pixel value as an ARGB integer (QRgb is a typedef for unsigned int)
    for(int i = 1; i < image.width()-1;i++){
        for(int j = 1; j < image.height()-1;j++){
            bool local_window[3][3] = {false};
            for(int m=-1;m<=1;m++){
                for(int n=-1;n<=1;n++){
                    QRgb rgbValue = image.pixel(i+m, j+n);
                    local_window[m+1][n+1] = (rgbValue != 0xffffffff);
                }
            }

            CustomMatrix mat(local_window);

            if (!isMatrixEmpty(mat)) {
                cout << "Window at (" << i << ", " << j << "):" << endl;
                printMatrix(mat);
            }

            string patternName = isSegmentPattern(mat);
            if (!patternName.empty()) {
                // Store the center point and pattern name of the detected segment
                m_detectedSegments.append({QPoint(i, j), patternName});
                cout << "--- MATCH: " << patternName << endl;
            }

            windows.push_back(mat);
        }
    }
    cout << "****************************************" << endl;
    cout << "Total 3x3 windows analyzed: " << windows.size() << endl;
    cout << "Total candidates detected: " << m_detectedSegments.size() << endl;
    cout << "--------------------------------\n" << endl;

    update();

    return;
}

void DrawingCanvas::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Set up the pen and brush for drawing the points
    QPen pen(Qt::blue, 5);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::blue));

    // Draw a small circle at each stored point
    for (const QPoint& point : std::as_const(m_points)) {
        painter.drawEllipse(point, 3, 3);
    }

    if(isPaintLinesClicked){
        cout << "paint lines block is called" << endl;
        pen.setColor(Qt::red);
        pen.setWidth(4); // 4-pixel wide line
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);

        // Set the painter's pen to our custom pen.
        painter.setPen(pen);

        for(int i=0;i<m_points.size()-1;i+=2){
            //cout << m_points[i].x() << endl;
            painter.drawLine(m_points[i], m_points[i+1]);
        }


        //return painter pen to blue
        pen.setColor(Qt::blue);
        painter.setPen(pen);
    }

    if (!m_detectedSegments.empty()) {

        QColor purple("#800080");
        pen.setColor(purple);

        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush); // Don't fill the rectangle

        const int RECT_SIZE = 5; // A slightly larger rectangle for visibility

        for (const auto& segment : std::as_const(m_detectedSegments)) {
            const QPoint& center = segment.first;
            painter.drawRect(
                center.x() - RECT_SIZE / 2,
                center.y() - RECT_SIZE / 2,
                RECT_SIZE,
                RECT_SIZE
            );
        }
    }
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event) {
    // Add the mouse click position to our vector of points
    m_points.append(event->pos());
    m_detectedSegments.clear();
    // Trigger a repaint
    update();
}


